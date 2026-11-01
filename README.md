# Jump Buffer
Cube jump buffer for Geometry Dash (Geode). A press made up to N ms before landing fires on landing.
A button on the main menu (bottom row) opens the settings. No pause-menu button, no watermark.

## Get the .geode file (no installs needed)
1. Create a GitHub repo and upload everything in this folder (keep `.github/`).
2. Change `id` and `developer` in mod.json first.
3. Open the Actions tab -> "Build Mod" -> Run workflow.
4. When it finishes, download "Build Output" -> the `.geode` is inside.
5. Put the .geode in your Geometry Dash `geode/mods` folder.

## Build locally
Install the Geode SDK + CLI, then run `geode build`.
