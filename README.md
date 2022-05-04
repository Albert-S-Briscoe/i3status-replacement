This project aimed to replace a script I made a few years ago, which replaced i3status, and I'm calling it a success.
It shows all the info I want at the bottom of my screen, in the colors and update rates I want.

If you, for some reason, run across this repository looking for a good i3status replacement, this probably isn't it.
It has oddities like support for two batteries and shows UTC (along with the local time),
but doesn't have much configurability unless you want to modify the code for ordering json objects or want this exact configuration.

That said, if you do want to use this for some reason, create config.h using laptop_config.h or desktop_config.h as a template, or symlink one to config.h with 
```
ln -s laptop_config.h config.h
```
Then run `make`, copy the executable (named status) somewhere convenient, and set it to your i3status command in i3's config.
