# HEYKUBE Sound Generator

This Arduino project give you the ability to play sounds and sound effects on an Arduino Uno board using a piezo buzzer, and using TIMER1 pulse-width modulation (PWM) mode to play the notes. We've developed the HEYKUBE by starting with tools and resource like Arduino board from the open-source community, so we'd like to share the code back with everyone. We've chosen the MIT license, feel free to copy and use the code. 

## Getting Started

This project is simple, download the Arduino project, and hook-up a piezo buzzer to pin 9 on your Arduino Uno board. In the code there are 5 defined sounds that can be played, and anywhere you see the word "CUSTOM", you can edit your own music or sound effects

```
// Define the songs
#define SONG_A              0x1
#define SONG_B              0x2
#define SONG_CUSTOM         0x3
#define SOUND_EFFECT_A      0x4
#define SOUND_EFFECT_CUSTOM 0x5
```

In the loop() method, you can edit the list of sounds to play, especially if you want to focus on your sound effect.

```
// TODO -  Edit your play list here
uint8_t play_list[] = { SONG_A, SOUND_EFFECT_A, SONG_B }; 
//uint8_t play_list[] = { SONG_A, SONG_B, YOUR_SONG, SOUND_EFFECT_A, YOUR_SOUND_EFFECT}; 
```

### Playing MUSIC 

The simplest method to create sounds is to play a series of notes, using this format:

```
uint16_t success_notes[] = {NOTE_C5, QUARTER_NOTE,  NOTE_REST, QUARTER_NOTE, NOTE_C5, QUARTER_NOTE, NOTE_REST, QUARTER_NOTE, 
                            NOTE_C5, QUARTER_NOTE,  NOTE_REST, QUARTER_NOTE, NOTE_G5, EIGHTH_NOTE | DOTTED_NOTE, NOTE_REST, EIGHTH_NOTE | DOTTED_NOTE, 
                            NOTE_C5, HALF_NOTE | EIGHTH_NOTE, NOTE_C5, HALF_NOTE | EIGHTH_NOTE, NOTE_D5, WHOLE_NOTE | QUARTER_NOTE};
```

The sequence is always the note frequency (like NOTE_C5 is defined at 523 Hz) followed by the duration.  The duration is a union of the whole notes, quarter notes, half notes, eighth and sixteen notes. If the DOTTED_NOTE is added to the duration, the entire note is stretched by 50% longer cycle. 

The tempo variable sets to overall speed of the song measured in quarter notes beats per minute. 

There is also the sound_class.rest variable, which enables a note to only play for 90% of the period, and rest. This models more like piano with a note strike, and transition to the next note. 


### Sound Effects

Just playing songs and notes can be limiting, you may want to use equations to create the awesome laser sounds effects, or have custom transitions between frequencies. 

The following example shows how to ramp from 300 Hz to 1050 Hz, and then 800 Hz back to 500 Hz. 


```
        // set number of notes
        sound_class.num_notes = 71;
                
        // Charging up 
        if (sound_class.note_index < 50) { 
            // Setup PWM frequency 
            freq = 300 + sound_class.note_index*15;
            sound_class.duration = 15000;
        } else if (sound_class.note_index == 50) { 
            freq = 0;
            sound_class.duration = 500000;
        } else if (sound_class.note_index < 71) { 
            freq = 800 - ((sound_class.note_index-51)*15);
            sound_class.duration = 10000;
        }
        sound_class.note_index++;
```
## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details
