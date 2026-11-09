# AI Boss Battle & Player Mechanics

A desktop Unreal Engine 5.3 prototype built around a T. rex boss encounter. The C++ project separates boss states, player abilities, weapon logic, and interactables so each system can be explored independently.

![Boss battle prototype preview](https://github.com/user-attachments/assets/c6f57af7-c78e-4e3c-a8a5-bc1101fe707e)

## Code tour

| System | Where to look |
| --- | --- |
| Boss behavior | [EnemyAI](Source/jurassic_BB_LD/Private/EnemyAI) contains the T. rex controller, state component factory, and individual attack, defense, hunt, exhaust, rage, and stun states. |
| Player abilities | [Player](Source/jurassic_BB_LD/Private/Player) includes the player character, dash, telekinesis, and tripwire components. |
| Equipment and interaction | [Weapon](Source/jurassic_BB_LD/Private/Weapon) and [Interactables](Source/jurassic_BB_LD/Private/Interactables) contain firearms, magazines, pickups, and objects used by the abilities. |

## Open the encounter

1. Clone the repository with Git LFS available for Unreal assets.
2. Open [jurassic_BB_LD.uproject](jurassic_BB_LD.uproject) with **Unreal Engine 5.3**. On Windows, install the Visual Studio C++ game development tools and build the editor target if prompted.
3. Open **Content → MainGame → Maps → TestLevel** and press **Play**. This is the configured editor startup map.

See [LICENSE](LICENSE) for the repository license.
