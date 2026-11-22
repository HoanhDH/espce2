# 🕹️ CS2 External ESP | Recode

Simple external ESP for Counter-Strike 2. After years of development the codebase has been modernized for clarity and ease of use, featuring a redesigned interface, noticeable performance improvements, several quality-of-life additions, and automatic offset scanning to help maintain compatibility through game updates.

## Offline click practice

Run `cs2-external-esp.exe --trigger-demo` to open the native practice window. Hold F6 over Head or Body for 150 ms to send one left click to that button. Release F6 before the next click. The counter updates when the button receives the click. To use your configured hotkey, delay, zone and activation mode, choose **Open offline click practice** in the **Trigger Demo** tab.

The **Mouse settings** section displays the sensitivity saved in the active Steam account's CS2 user configuration. If automatic detection is unavailable, use **Open .vcfg...** to select your `cs2_user_convars_0_slot0.vcfg`. Changes made in the game appear here after CS2 saves them and you reopen the file or practice window.

For a mouse DPI estimate, mark a straight horizontal distance of 10 cm on your mouse pad. Set the mouse at one end, select **Measure DPI**, move to the other end without lifting it, and press **Space**. The dialog estimates DPI from relative mouse movement over that distance. Switching windows cancels the measurement.

To build just the offline demo from the repository root using a Visual Studio Developer Command Prompt:

```bat
rc /fo trigger-demo.res src\gui\frontend\menu\practice\ClickPractice.rc
cl /EHsc /std:c++20 tools\trigger-demo.cpp trigger-demo.res user32.lib advapi32.lib comdlg32.lib
trigger-demo.exe
```

## Showcase

> Click the picture below to go to the showcase video

[![cs2esp](.github/showcase.png)](https://youtu.be/3WHHLUyHyzA)

## 🌳 Simple Use

- You can download it from [**Releases**](https://github.com/IMXNOOBX/cs2-external-esp/releases) tab or **build it yourself** by following [developers instructions](#-developer-instructions).
- Open the game & the `cs2-external-esp.exe`, and thats it!
- **Star** the repository **if** you **like the project**! ⭐⭐⭐

> If the app crashes on startup, you may be missing the latest [Windows Visual C++ redistributables](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist?view=msvc-170#latest-supported-redistributable-version). Install the appropriate package for your system:
>
> - [64-bit Windows](https://aka.ms/vc14/vc_redist.x64.exe) *for most modern systems*
> - [32-bit](https://aka.ms/vc14/vc_redist.x86.exe) *for older devices*


> [!IMPORTANT]
> Make sure your game is in full screen windowed❗

## 💡 Important

> This project is provided *'as is'* for learning purposes with no warranties or responsibility from the developers/contributors. Use it at your own risk; you are the only one accountable for your actions

* **Detection Status:** This project is intended solely for single-player use. That said, no ban reports have been raised for other modes.
* **Anti-Virus Alerts:** This software may resemble malware in behavior because it accesses other processes memory, so it is commonly flagged by anti‑virus programs. I strongly encourage you to read the source code and build it yourself by following the [developers instructions](#-developer-instructions). All provided binaries are compiled via the [GitHub workflow](.github/workflows/auto_build.yml) from the repository source.

## 🕹️ Previous Versions

> This project has been reworked **3 times**, the current one been the third!

* [Discord Overlay (2023)](https://github.com/IMXNOOBX/cs2-external-esp/tree/discord-overlay) is the first and the **simplest** version of all, great to start learning.
* [Gdi Overlay (2023-2025)](https://github.com/IMXNOOBX/cs2-external-esp/tree/gdi-overlay) is an **improved version**, featuring automatic offset updating & configurations
* [Modern Version (Today)](https://github.com/IMXNOOBX/cs2-external-esp/tree/main) is the current and the **latest version**, with a click ui, automatic offset scanning and more!

## 📘 Developer Instructions

> - This project is mirrored in the following locations.
>	 - **GitHub**: [*github.com/IMXNOOBX/cs2-external-esp*](https://github.com/IMXNOOBX/cs2-external-esp) (main)
> 	 - **GitLab**: [*gitlab.com/IMXNOOBX/cs2-external-esp*](https://gitlab.com/IMXNOOBX/cs2-external-esp) (mirror)
>	 - **CodeBerg**: [*codeberg.org/IMXNOOBX/cs2-external-esp*](https://codeberg.org/IMXNOOBX/cs2-external-esp) (mirror)

1. Clone repository. Make sure you copy the command below to clone dependencies too

```sh
git clone --recursive https://github.com/IMXNOOBX/cs2-external-esp
```

* If you cloned the repository before submodules were added, run this command `git submodule update --init --recursive`

2. Build the app using **Visual Studio 2026**
	- Build: **`x64 - Release`**

3. Locate your binary file in the folder `<arch>/<configuration>`, e.g., `x64/Release`.

## 💫 Credits

* All [contributors](https://github.com/IMXNOOBX/cs2-external-esp/graphs/contributors) who have helped improve the project!
* [**a2x**](https://github.com/a2x) for his [offset dumper](https://github.com/a2x/cs2-dumper) and constant updates to it!

## 🔖 License & Copyright

This project is licensed under [**CC BY-NC 4.0**](https://creativecommons.org/licenses/by-nc/4.0/).

```diff
+ You are free to:
	• Share: Copy and redistribute the material in any medium or format.
	• Adapt: Remix, transform, and build upon the material.
+ Under the following terms:
	• Attribution: You must give appropriate credit, provide a link to the original source repository, and indicate if changes were made.
	• Non-Commercial: You may not use the material for commercial purposes.
- You are not allowed to:
	• Sell: This license forbids selling original or modified material for commercial purposes.
	• Sublicense: This license forbids sublicensing original or modified material.
```

### ©️ Copyright
The content of this project is ©️ by [IMXNOOBX](https://github.com/IMXNOOBX) and the respective contributors. See the [LICENSE.md](LICENSE) file for details.
