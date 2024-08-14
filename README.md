### Backie

Backie is a desktop app for managing incremental backups, similar to Git but designed to work with all kinds of files, not just text. It also features a more user-friendly interface, which was put together by my friend [CogniSeeker](https://github.com/CogniSeeker) using Qt Creator. On my side, I handled the backend, setting up things like scheduling backups with Windows Task Manager, and managing all the metadata for those backups.

The project was a real learning experience for us. We were pretty new to this kind of work and didn’t know about the libraries that could have made our lives easier. So, apart from using the nlohmann json library and spdlog for logging, I wrote everything from scratch. It was challenging, but it really helped us grow our skills. Learning OOP more in-depth and diving into its programming patterns was especially interesting.

One thing to note is that there aren't any "running" instructions included because the compiled .exe file doesn’t actually do anything. The project was never fully functional, and at no point was it even remotely ready for use. However, you can still look at the code—everything in the `src` directory was written by me.
