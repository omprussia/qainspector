# Inspection tool companion for qapreload

Can be used for inspecting element tree of applications with injected qapreload library.

## prepare

```
sudo add-apt-repository ppa:alexlarsson/flatpak
sudo apt install flatpak flatpak-builder

flatpak remote-add --user --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
flatpak install --user flathub org.kde.Sdk/x86_64/5.11 org.kde.Platform/x86_64/5.11
```

## build

```
flatpak-builder --user --install-deps-from=flathub --force-clean --repo=repo build ru.omprussia.qainspector.json
flatpak build-bundle --runtime-repo=https://flathub.org/repo/flathub.flatpakrepo $(pwd)/repo ru.omprussia.qainspector.flatpak ru.omprussia.qainspector stable
```

## install

```
flatpak --user install ru.omprussia.qainspector.flatpak
```

## run

```
flatpak run ru.omprussia.qainspector
```

Run dummy test for your application, while application is launched with Appium `bridge` connect to it using this tool.

Application could be launched manually with `LD_PRELOAD` or injecting `libqapreloadhook` library to executable. Make sure to put `libqaengine` library in the same folder with 'libqapreloadhook'

