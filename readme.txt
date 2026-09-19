[h1]What Is This?[/h1]

This mod adds a player dirtiness mechanic to the game.

Over time, the player gets dirty from performing various actions (configurable in the server mod config).

When a player reaches a certain dirtiness threshold, their hands become filthy (dirt texture is applied like the bloody hands texture).

When a player reaches full 100% dirty, a small hand badge sickness icon appears on their HUD, and they now have a chance to randomly spawn flies on them for a few minutes every ~15-20 minutes (which can give their position away in PVP etc). Their immune system also becomes compromised and their immunity level has a -50% debuff applied, making it easier to get sick (compatible with Terje's Medicine mod).

To clean themselves they'll need to do one of the following (configurable in the server config):
- Wash themselves at a well
- Wash themselves in a natural water source
- Wash themselves with a bottle/canteen/container of water (least effective way to clean yourself)
- Submerge themselves fully into water (ie. go swimming: this instantly washes the player to full clean)

The dirty hands texture will apply regardless of whether gloves are on or not, but it's purely cosmetic. The bloody hands texture takes priority so if the player has bloody hands that will be the texture displayed since that is a higher priority situation. Cleaning your hands does NOT clean your character - cleaning the character is a separate continuous action.

Native compatibility with Community Online Tools is included for server admins: you can set the dirtiness level of players in the Player Management GUI.

It also includes chat commands for admins who add themselves to my ZenModCore.json admin list:
!togglezendirty <optionalPlayerID>
!setzendirty <amount> <optionalPlayerID>
!getzendirty <optionalPlayerID>

[h1]Installation Instructions:[/h1]

Install this mod like any other mod - copy it into your server folder and add it to your mods list. Make sure to copy the .bikey into your server keys if you're not using a server management tool like OmegaManager which does that automatically.

This mod requires my [ZenModCore](https://steamcommunity.com/sharedfiles/filedetails/3702420204) to function.

This mod must be run on both client and server. If you are running my ZenModPack (Zen's Enormous Package) you will also need to enable the mod inside ZenModPackConfig.json under 'ZenDirty', as it's disabled by default in my modpack.

[h1]Repack & Source Code:[/h1]

You can repack this mod if you like, and do anything else you want with it for that matter. Just keep in mind my future updates won't be applied so make sure to check back for new versions if you notice any bugs. The source code is on my GitHub at www.zenarchist.com

[h1]Learn Modding[/h1]

Want to learn how to make your own mods? Check out my guides on YouTube: https://www.youtube.com/@Zenarchist

[h1]Buy Me A Coffee:[/h1]

All my mods are free and open source, but it takes an enormous amount of time to put some of these mods together. If I've helped you out, please consider helping me buy my next coffee! I don't expect it, but I very much appreciate it.

https://buymeacoffee.com/zenarchist

Enjoy