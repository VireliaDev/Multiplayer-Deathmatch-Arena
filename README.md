# \# Multiplayer Deathmatch Arena

# 

# A fast-paced third-person deathmatch arena shooter for 2–8 players, built solo in Unreal Engine 5 with C++ and the Gameplay Ability System (GAS).

# 

# \*\*Status:\*\* Pre-production (design and planning)

# 

# \[Design doc](https://docs.google.com/document/d/1DuCkFCHjMxGcoANQjauKPUlrU714bcXGE3BWMM9Bz00) | \[Project page and devlogs](https://vireliadev.github.io/projects/mda/)

# 

# \## About

# 

# Two to eight players spawn into an arena in a free-for-all. The first to 25 kills wins, or whoever has the most kills when time runs out. Weapons are hitscan with bloom-based accuracy, health doesn't regenerate but shields do, and players pick their loadout between lives.

# 

# \## What this project demonstrates

# 

# \- \*\*Networking:\*\* server-authoritative gameplay, replication, prediction within GAS, and testing under poor network conditions

# \- \*\*Hit registration:\*\* client-side hit detection, validated by the server

# \- \*\*Gameplay Ability System:\*\* attributes (health and shield), abilities (weapons), tags and effects

# \- \*\*Data-driven design:\*\* weapon and movement values configured in data, not hard-coded

# \- \*\*Game state:\*\* match phases, respawning and late joiners

# \- \*\*Process:\*\* planning, scoping, milestones and documentation

# 

# \## Features (planned)

# 

# \- Third-person over-the-shoulder camera

# \- Movement: sprint, jump, crouch, slide and mantle

# \- Weapons: handgun, assault rifle and shotgun, with a marksman rifle and hand cannon as stretch goals

# \- Shield and health system with shield recharge

# \- Match flow: warmup, countdown, match, post-match scoreboard

# \- HUD, kill feed, scoreboard and weapon selection menu

# \- Listen-server hosting by IP (no Steam or platform integration); code kept compatible with dedicated servers

# 

# 



# Progress is tracked in the design doc and written up in the devlogs.

# 

# 

# 

# \*\*Note:\*\* third-party art (Paragon and Fab assets) is not included in this repo, as their licences don't allow redistribution. Project may fail to work properly without these assets.

# 

# \## Licence

# 

# Code is released under the \[MIT License](LICENSE). Copyright (c) 2026 Sidney Levin.

