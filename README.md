# Tsunami Mk 2

Send a wave, capture the shore. A game for Blinks. https://blinks.games/

Players: As many as can fit around the table/cluster of blinks.

Teams: Up to 5 teams; `Red`, `Yellow`, `Green`, `Blue`, and `Magenta`

Blinks: 24 - infinity  

Playtime: short, to be determined

Try to claim the shore with waves of your team's color.

## Setup

Each player chooses a team color: `Red`, `Yellow`, `Green`, `Blue`, or `Magenta`.

Place the blinks in one large cluster.

## Play

When an `Ocean` tile becomes a `Wave Trigger` of your team's color, click it to send a wave of your color.

## Victory

When there are no more `Ocean` tiles, count up the `Claimed Shore` tiles of each color. The team color with the most `Claimed Shore` tiles wins.

## Reset

Long press any blink to send a reset wave that reinitializes all blinks to `Ocean` tiles.


## Tiles

### Ocean

Blinks start as `Ocean` tiles, which propogate waves.

Randomly become `Wave Trigger` tiles.

### Wave Trigger

Allow triggering a wave of the indicated team color

Reverts to `Ocean` after two seconds.

Single click -> Send Wave of the indicated color

### Open Shore

Blinks on the edge will show a white border indicating they are open shore tiles available to be claimed by the next wave to crash into them.

When a wave crashes into an `Open Shore`, the blink becomes `Claimed Shore` for the team color of the crashing wave.

### Claimed Shore

Locked to the team color of the wave that crashed into it.

## Increase Ocean Size

Make small "holes" in the ocean.

Remove one or two adjacent blinks from inside the cluster to the edge of the cluster.

Make sure "holes" are no bigger than two blinks in size.

## Known Issues

- Timers need to be tuned. Various cooldowns to tweak.
- Only 5 colors enabled.
- Colors 6 and 7 cause weird propagation effects.
- Animations need lots of work.
- Waves of the same color will cancel eachother out.
- Large holes in the ocean (3-4+ tiles) will create continuous wave generators.
