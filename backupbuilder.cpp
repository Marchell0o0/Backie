#include "backupbuilder.h"

#include "settings.h"
#include "spdlog/spdlog.h"

void BackupBuilder::reset() {
    this->name = "";
    this->currentType = BackupType::NONE;
    this->destinationsKeys.clear();
    this->sources.clear();
    this->schedules.clear();
}

BackupBuilder& BackupBuilder::setName(const std::string &name) {
    this->name = name;
    return *this;
}

BackupBuilder& BackupBuilder::setCurrentType(const BackupType currentType) {
    this->currentType = currentType;
    return *this;
}

BackupBuilder& BackupBuilder::setDestinations(const std::vector<Destination> destinations) {
    for (auto& destination : destinations) {
        this->destinationsKeys.push_back(destination.getKey());
    }
    return *this;
}

BackupBuilder& BackupBuilder::setDestinations(const std::vector<std::string> destinationsKeys) {
    this->destinationsKeys = destinationsKeys;
    return *this;
}

BackupBuilder& BackupBuilder::setSources(const std::vector<std::filesystem::__cxx11::path> sources) {
    this->sources = sources;
    return *this;
}

BackupBuilder& BackupBuilder::setSchedules(std::vector<std::shared_ptr<Schedule> > schedules) {
    this->schedules = schedules;
    return *this;
}

BackupBuilder& BackupBuilder::setKey(const std::string &key){
    this->key = key;
    return *this;
}

BackupBuilder& BackupBuilder::setNoNewKey() {
    this->noNewKey = true;
    return *this;
}

std::optional<Errand> BackupBuilder::buildErrand() {

    if (this->currentType == BackupType::NONE) {
        SPDLOG_ERROR("No current type specified");
        return std::nullopt;
    }

    if (this->key == "") {
        SPDLOG_ERROR("Key not specified");
        return std::nullopt;
    }

    Settings& settings = Settings::getInstance();
    if (!settings.isTaskKeyInSettings(key)) {
        SPDLOG_ERROR("Specified key isn't in settings");
        return std::nullopt;
    }

    this->destinationsKeys = settings.getKeyDests(key);
    this->sources = settings.getKeySrcs(key);
    this->name = settings.getKeyName(key);

    if (this->destinationsKeys.empty()){
        SPDLOG_ERROR("No destinations found for this key");
        return std::nullopt;
    }
    if (this->sources.empty()){
        SPDLOG_ERROR("No sources found for this key");
        return std::nullopt;
    }
    //TODO:
    if (this->name.size() < 3) {
        SPDLOG_ERROR("Name is too small");
        return std::nullopt;
    }

    //TODO: Just remove any bad sources and destiantions
    // and if at least one left - proceed
    for (auto& destinationKey : this->destinationsKeys) {
        auto dest = settings.getDest(destinationKey);
        if (!dest || !fs::exists(dest->destinationFolder) ||
            !fs::is_directory(dest->destinationFolder)) {
            SPDLOG_ERROR("Destination folder doesn't exist");
//            return std::nullopt;
        }
    }
    for (auto& source : this->sources) {
        if (!fs::exists(source) || !fs::is_directory(source)) {
            SPDLOG_ERROR("Source folder doesn't exist");
            return std::nullopt;
        }
    }

    Errand errand(key, name, currentType, destinationsKeys, sources);

    this->reset();

    return errand;
}


std::optional<Task> BackupBuilder::buildTask() {
    if (!this->noNewKey){
        this->key = generate_uuid_v4();
    }

    //TODO: other name checks
    if (this->name == "") {
        SPDLOG_ERROR("Name not specified or wrong format");
    }

    if (this->destinationsKeys.empty()){
        SPDLOG_ERROR("No destinations or dest keys specified");
        return std::nullopt;
    }
    if (this->sources.empty()){
        SPDLOG_ERROR("No sources specified");
        return std::nullopt;
    }
    if (this->schedules.empty()){
        SPDLOG_ERROR("No schedules specified");
        return std::nullopt;
    }

    //TODO: Just remove any bad sources, destiantions and schedules
    // and if at least one left - proceed
    Settings& settings = Settings::getInstance();
    for (auto& destinationKey : this->destinationsKeys) {
        auto dest = settings.getDest(destinationKey);
        if (!dest || !fs::exists(dest->destinationFolder) ||
            !fs::is_directory(dest->destinationFolder)) {
            SPDLOG_ERROR("Destination folder doesn't exist");
//            return std::nullopt;
        }
    }
    for (auto& source : this->sources) {
        if(!fs::exists(source) || !fs::is_directory(source)){
            SPDLOG_ERROR("Source folder doesn't exist");
            return std::nullopt;
        }
    }
    for (auto& schedule : schedules) {
        if (!schedule->verify()) {
            SPDLOG_ERROR("Schedule is wrong");
            return std::nullopt;
        }
    }

    Task task(key, name, currentType, destinationsKeys, sources, schedules);

    this->reset();

    return task;
}
