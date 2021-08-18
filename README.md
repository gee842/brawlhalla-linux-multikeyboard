# brawlhalla-linux-multikeyboard
Binding a second keyboard to a virtual controller for brawlhalla. Seems to have undetectable latency, I feel no difference in input (high diamond rank)

### Source
Taken and modified from here:

https://forums.bannister.org/ubbthreads.php?ubb=showflat&Number=118786

### Configs
Can be setup by modifying `translate.c` before build:
1. `Lines 45 to 60` Key Mapping
2. `Line 23` The location of your input device (See `evtest` to find out)
3. `Line 202` Event loop sleep interval (I find `5ms` to be pretty decent)

### Build
`cc translate.c -o translate`

### Run
`sudo ./translate [optional: path to input device]`

### Setup
Go to steam controller settings, and rebind the keymap to the standard brawlhalla controls for controller.





