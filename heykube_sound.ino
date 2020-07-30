/* =============================================================================
* MIT License
*
* Copyright (c) 2020 22nd Solutions, LLC
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
* ================================================================================*/
#include <stdint.h>
#include <stdbool.h>
    
// Uses TIMER1, pin A on Arduino Uno
#define PIEZO_PIN 9

// Sets 1kHz frame rate
#define FRAME_RATE 1000

// get constants in usecs
#define SOUND_TIMESTEP  (1000000 / FRAME_RATE)
#define PWM_FREQUENCY 16000000

// ------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------
// Code to hadle sound effects
// ------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------
        
// -----------------------------------------------------------------------------
// sound_class_type
//
//
//  select == SONG_TYPE(0) - Plays notes of songs 
//  num_notes   - sets the total number of of notes in the sequence
//  *melody     - points to the melody of the song - notes and duration in pairs
//  note_index  - holds index of current note
//  rest        - set to true, plays only 90% of note, 10% silence
//  duration    - holds duration of current note in usecs
//  time        - records the current time in usecs
//  tempo       - Sets the beats per minute for the quarter note
// 
//  select >= SOUND_EFFECT_A(1) - Plays custom frequuencies 
//  num_notes   - sets the total number steps in the sequence
//  note_index  - holds index of current note
//  duration    - holds duration of frequency in usecs
//  time        - records the current time in usecs
//
// --------------------------------------------------------------------------
// members
struct sound_class_type {
    int8_t   select;
    uint16_t *melody;
    uint16_t note_index;
    uint32_t duration;
    uint16_t tempo;
    bool     rest;
    uint32_t time;
    uint16_t num_notes;
};

// define holding class
struct sound_class_type sound_class;    
    
// ------------------------------------------
// Define the notes frequencies
// ------------------------------------------
#define NOTE_REST 0
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define REST      0

// define duration
#define DOTTED_NOTE    0x80
#define WHOLE_NOTE     0x01
#define HALF_NOTE      0x02
#define QUARTER_NOTE   0x04
#define EIGHTH_NOTE    0x08
#define SIXTEENTH_NOTE 0x10

// Define the songs
#define SONG_TYPE           0x0
#define SONG_A              0x1
#define SONG_B              0x2
#define SONG_CUSTOM         0x3
#define SOUND_EFFECT_A      0x4
#define SOUND_EFFECT_CUSTOM 0x5

// -----------------------------------------------------
// Initialize the sound class
// -----------------------------------------------------
void sound_clear() { 

    sound_class.tempo = 120;
    sound_class.melody = NULL;

    sound_class.select = 0;
    sound_class.rest = false;
    sound_class.note_index = 0;
    sound_class.duration = 0;
    sound_class.time = 0;
    sound_class.num_notes = 0;
  
}

// ---------------------------------------------
// This puts the frequency out on the pizeo
//
// Uses PWM mode at period/2 -- toggles the outpuput
// ---------------------------------------------
void play_freq_piezo(uint16_t freq) { 

  uint16_t period;

  if (freq > 0) { 
    period = (PWM_FREQUENCY / freq) >> 3;
    OCR1A = period;
    //TCCR1B |= _BV(CS11); // 8 prescaler 
    TCCR1B |= _BV(CS10);
    pinMode(PIEZO_PIN, OUTPUT);
  } else {
    //TCCR1B &= ~_BV(CS11); // shutoff counter
    TCCR1B &= ~(_BV(CS10));
    pinMode(PIEZO_PIN, INPUT);
  }

//  OCR1B = period >> 1; 
  //Serial.print("TCCR1B: ");
  //Serial.print(TCCR1B);
  //Serial.print("\n");
  
}

// ----------------------------------------------
// Generates the sequence of notes
// ----------------------------------------------
void update_piezo_tone() {
    uint16_t period;
    uint16_t freq;

    // subtract duration (non-accumulating error)
    sound_class.time -= sound_class.duration;

    // ends the sound
    if (sound_class.note_index >= sound_class.num_notes) {
        stop_sound();
        return;
    }
    

    // ---------------------------------------------------
    // Plays MUSIC
    // ---------------------------------------------------
    if (sound_class.select == SONG_TYPE) { 

        // Play the note reset
        if (sound_class.note_index & 0x1) { 
            freq = NOTE_REST;
            sound_class.duration /= 9;
        // Play the real notes
        } else { 
            uint8_t note_type;
            uint32_t quarter_note;
   
            // get note
            freq = sound_class.melody[sound_class.note_index];

            // set the quarter note duration (beats per minute into usec quarter note duration
            quarter_note = 60e6 / (uint32_t)sound_class.tempo;

            //  add up note duration
            sound_class.duration = 0;
            note_type = sound_class.melody[sound_class.note_index+1];           
            if (note_type & WHOLE_NOTE) {
              sound_class.duration += (quarter_note << 2);
            }
            if (note_type & HALF_NOTE) {
              sound_class.duration += (quarter_note << 1);
            }
            if (note_type & QUARTER_NOTE) {
              sound_class.duration += quarter_note;
            }
            if (note_type & EIGHTH_NOTE) { 
              sound_class.duration += (quarter_note >> 1);
            }
            if (note_type & SIXTEENTH_NOTE) { 
              sound_class.duration += (quarter_note >> 2);  
            }
            
            // stretch by 50% for dotted notes
            if (note_type & DOTTED_NOTE) { 
              sound_class.duration += sound_class.duration >> 1;
            }

            // Get to 90% period
            if (sound_class.rest) { 
              sound_class.duration *= 9;
              sound_class.duration /= 10;
            }

        }

        // if we rest for everyone note
        if (sound_class.rest) { 
          sound_class.note_index++;  
        } else {
          sound_class.note_index += 2;
        }
    
    // ---------------------------------------------------
    // This ramps 300 Hz to 800 Hz over 
    // ---------------------------------------------------
    }  else if (sound_class.select == SOUND_EFFECT_A) { 

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
 

    // ---------------------------------------------------
    // Your Sound Effect -- add your code here
    // ---------------------------------------------------
    }  else if (sound_class.select == SOUND_EFFECT_CUSTOM) { 

        // set number of notes
        sound_class.num_notes = 10;
        sound_class.duration = 100000;
        freq = NOTE_REST;

        // continues
        sound_class.note_index++;
    }


#ifdef CRAP
    // -----------------------------------------------
    // Plays a sound
    // -----------------------------------------------
    if (sound_class.select == 1) { 
        
        // clear time
        sound_class.rest = false;
        sound_class.time -= sound_class.duration;
        
        uint16_t notes[] =     {500, 0,   500,   0, 500, 0,  800, 0,   500, 0,  600};
        uint16_t duration[] = {200, 200, 200, 200, 200, 200, 150, 150, 500, 500, 1000};
            
        if (sound_class.note_index < 11) {
         
            if (notes[sound_class.note_index] == 0) { 
                period = 0;  
            } else {
                period = PWM_FREQUENCY / notes[sound_class.note_index];
            }
            sound_class.duration = duration[sound_class.note_index++]*1000;
            
        } else {
            sound_class.note_index = -1;
            period = 0;
            sound_class.duration = 0;
            sound_class.time = 0;
        }
    } else if (sound_class.select == 2) { 
        
        // clear time
        sound_class.rest = false;
        sound_class.time -= sound_class.duration;
        
        uint32_t num_notes = 12;
        uint16_t notes[] =     {NOTE_C4,0, NOTE_C3,0, NOTE_G3, 0, NOTE_C4,0,
                                NOTE_E4, 0, NOTE_DS4,0,
                                NOTE_G1, 0, NOTE_C2, 0,
                                NOTE_G1, 0, NOTE_C2, 0,
                                NOTE_G1, 0, NOTE_C2, 0,
                                NOTE_G1, 0, NOTE_C2, 0,
                                NOTE_G1, 0, NOTE_C2, 0,                    
                                NOTE_G1, 0, NOTE_C2, 0,
                                NOTE_C3,0, NOTE_G3, 0, NOTE_C4,0,
                                NOTE_E4, 0, NOTE_DS4,0};
        
        
        
        uint16_t duration[] =  {200,200,1000, 200,  1000, 200, 2000,  200,
                                200, 200, 2000, 200, 
                                90, 90, 90, 90,
                                90, 90, 90, 90,
                                90, 90, 90, 90,
                                90, 90, 90, 90,
                                90, 90, 90, 90,
                                90, 90, 90, 90,
                                1000, 200,  1000, 200, 2000,  200,
                                200, 200, 2000, 200}; 
            
            
        
            
        if (sound_class.note_index < num_notes) {
         
            if (notes[sound_class.note_index] == 0) { 
                period = 0;  
            } else {
                period = PWM_FREQUENCY / notes[sound_class.note_index];
            }
            sound_class.duration = duration[sound_class.note_index]*1000;
            sound_class.note_index++;
            
        } else {
            sound_class.note_index = -1;
            period = 0;
            sound_class.duration = 0;
            sound_class.time = 0;
        }
       
    } else if (sound_class.select == 3) { 
        
        // clear time
        sound_class.rest = false;
        sound_class.time -= sound_class.duration;
            
        // triangle wave
        if (sound_class.note_index < BEEP_DURATION) { 
            // Setup PWM frequency 
            sound_class.duration = 2000;
            period = PWM_FREQUENCY / ( BEEP_PITCH + (sound_class.note_index*BEEP_RANGE)/BEEP_DURATION);
            sound_class.note_index++;
        } else {
            sound_class.note_index = -1;
            period = 0;
            sound_class.duration = 0;
            sound_class.time = 0;
        }    
        
    } else if (sound_class.select == 4) {
     
        // notes of the moledy followed by the duration.
        // a 4 means a quarter note, 8 an eighteenth , 16 sixteenth, so on
        // !!negative numbers are used to represent dotted notes,
        // so -4 means a dotted quarter note, that is, a quarter plus an eighteenth!!
        int16_t melody[] = {
          
          // Dart Vader theme (Imperial March) - Star wars 
          // Score available at https://musescore.com/user/202909/scores/1141521
          // The tenor saxophone part was used
          
          NOTE_A4,-4, NOTE_A4,-4, NOTE_A4,16, NOTE_A4,16, NOTE_A4,16, NOTE_A4,16, NOTE_F4,8, REST,8,
          NOTE_A4,-4, NOTE_A4,-4, NOTE_A4,16, NOTE_A4,16, NOTE_A4,16, NOTE_A4,16, NOTE_F4,8, REST,8,
          NOTE_A4,4, NOTE_A4,4, NOTE_A4,4, NOTE_F4,-8, NOTE_C5,16,

          NOTE_A4,4, NOTE_F4,-8, NOTE_C5,16, NOTE_A4,2,//4
          NOTE_E5,4, NOTE_E5,4, NOTE_E5,4, NOTE_F5,-8, NOTE_C5,16,
          NOTE_A4,4, NOTE_F4,-8, NOTE_C5,16, NOTE_A4,2,
          
          NOTE_A5,4, NOTE_A4,-8, NOTE_A4,16, NOTE_A5,4, NOTE_GS5,-8, NOTE_G5,16, //7 
          NOTE_DS5,16, NOTE_D5,16, NOTE_DS5,8, REST,8, NOTE_A4,8, NOTE_DS5,4, NOTE_D5,-8, NOTE_CS5,16,

          NOTE_C5,16, NOTE_B4,16, NOTE_C5,16, REST,8, NOTE_F4,8, NOTE_GS4,4, NOTE_F4,-8, NOTE_A4,-16,//9
          NOTE_C5,4, NOTE_A4,-8, NOTE_C5,16, NOTE_E5,2,

          NOTE_A5,4, NOTE_A4,-8, NOTE_A4,16, NOTE_A5,4, NOTE_GS5,-8, NOTE_G5,16, //7 
          NOTE_DS5,16, NOTE_D5,16, NOTE_DS5,8, REST,8, NOTE_A4,8, NOTE_DS5,4, NOTE_D5,-8, NOTE_CS5,16,

          NOTE_C5,16, NOTE_B4,16, NOTE_C5,16, REST,8, NOTE_F4,8, NOTE_GS4,4, NOTE_F4,-8, NOTE_A4,-16,//9
          NOTE_A4,4, NOTE_F4,-8, NOTE_C5,16, NOTE_A4,2,
          
        };
        sound_class.num_notes = sizeof(melody) / sizeof(melody[0]);
        

        // clear time
        sound_class.rest = false;
        sound_class.time -= sound_class.duration;
        
        // find the period
        period = play_note_sequence(melody, sound_class.num_notes);
           
    } else if (sound_class.select == 5) {
     
        // notes of the moledy followed by the duration.
        // a 4 means a quarter note, 8 an eighteenth , 16 sixteenth, so on
        // !!negative numbers are used to represent dotted notes,
        // so -4 means a dotted quarter note, that is, a quarter plus an eighteenth!!
        int16_t melody[] = {
          NOTE_CS4,8, NOTE_E4, 8, NOTE_E4, 8, NOTE_FS4, 8, NOTE_E4, 4, 
          NOTE_D4,8, NOTE_CS4, 8, NOTE_B3, 8, NOTE_A3, 8, NOTE_GS3, -4, NOTE_A3, -4,
          // second refrain
          NOTE_CS4,8, NOTE_E4, 8, NOTE_E4, 8, NOTE_FS4, 8, NOTE_E4, 4, 
          NOTE_D4,8, NOTE_CS4, 8, NOTE_B3, 8, NOTE_A3, 8, NOTE_GS3, -4, NOTE_A3, -4    
        };
        sound_class.num_notes = sizeof(melody) / sizeof(melody[0]);
       
        // clear time
        sound_class.rest = false;
        sound_class.time -= sound_class.duration;
        
        // find the period
        period = play_note_sequence(melody, sound_class.num_notes);
            
     
    } else { 

        period = 0;
        sound_class.note_index = -1;
        
    }
#endif

    // setup Period
    play_freq_piezo(freq);

}

// ---------------------------------------
// Check if it's playing
// ---------------------------------------
bool sound_is_playing() { 
    if ((sound_class.note_index < sound_class.num_notes) || (sound_class.duration > 0)) { 
        return true;   
    } else { 
        return false;   
    }
}

// ------------------------------------------------
// Stops playing sound
// ------------------------------------------------
void stop_sound() { 
  sound_clear();
  play_freq_piezo(NOTE_REST);
}

// ---------------------------------------
// Run the sound generation
// ---------------------------------------
void sound_run() {
    
    // start the douns 
    if (sound_is_playing()) { 
     
        // check the duration
        if (sound_class.time >= sound_class.duration) {
            update_piezo_tone();
        }

        // update time in microseconds
        sound_class.time += SOUND_TIMESTEP;
    }
    
}

// -----------------------------------------------
// Plays the song succcess
// -----------------------------------------------
uint16_t success_notes[] = {500, QUARTER_NOTE,  NOTE_REST, QUARTER_NOTE, 500, QUARTER_NOTE, NOTE_REST, QUARTER_NOTE, 
                            500, QUARTER_NOTE,  NOTE_REST, QUARTER_NOTE, 800, EIGHTH_NOTE | DOTTED_NOTE, NOTE_REST, EIGHTH_NOTE | DOTTED_NOTE, 
                            500, HALF_NOTE | EIGHTH_NOTE, 500, HALF_NOTE | EIGHTH_NOTE, 600, WHOLE_NOTE | QUARTER_NOTE};


uint16_t that_way_notes[] = { NOTE_CS4, EIGHTH_NOTE, NOTE_E4,  EIGHTH_NOTE, NOTE_E4, EIGHTH_NOTE, NOTE_FS4, EIGHTH_NOTE, NOTE_E4, QUARTER_NOTE, 
                              NOTE_D4,  EIGHTH_NOTE, NOTE_CS4, EIGHTH_NOTE, NOTE_B3, EIGHTH_NOTE, NOTE_A3,  EIGHTH_NOTE, NOTE_GS3, QUARTER_NOTE | DOTTED_NOTE, NOTE_A3, QUARTER_NOTE | DOTTED_NOTE,
                              // second refrain
                              NOTE_CS4,EIGHTH_NOTE, NOTE_E4, EIGHTH_NOTE, NOTE_E4, EIGHTH_NOTE, NOTE_FS4, EIGHTH_NOTE, NOTE_E4, QUARTER_NOTE, 
                              NOTE_D4,EIGHTH_NOTE, NOTE_CS4, EIGHTH_NOTE, NOTE_B3, EIGHTH_NOTE, NOTE_A3, EIGHTH_NOTE, NOTE_GS3,  QUARTER_NOTE | DOTTED_NOTE, NOTE_A3,  QUARTER_NOTE | DOTTED_NOTE };   
            
// CUSTOMIZE YOUR SONG HERE
uint16_t custom_song[] = { NOTE_REST, WHOLE_NOTE}; 


// print names
void print_sound_name(uint8_t select) { 
  if (select == SOUND_EFFECT_A) {
    Serial.print("Sound Effect A");
  } else if (select == SOUND_EFFECT_CUSTOM) { 
    Serial.print("Custom sound effect\n"); 
  } else if (select == SONG_A) { 
    Serial.print("Song A"); 
  } else if (select == SONG_B) { 
    Serial.print("Song B"); 
  } else if (select == SONG_CUSTOM) { 
    Serial.print("Custom song"); 
  } 
  
}


// -------------------------------------------------
// Selects from sound effects
// -------------------------------------------------
void play_sound(uint8_t select) {

    sound_clear();

    // setup custom sound effects
    if (select == SOUND_EFFECT_A) {
      sound_class.select = select;
      sound_class.num_notes = 1;
    } else if (select == SOUND_EFFECT_CUSTOM) { 
      sound_class.select = select;
      sound_class.num_notes = 1; 
    // Play songs - success sound 
    } else if (select == SONG_A) { 
      sound_class.tempo = 300;
      sound_class.melody = success_notes;
      sound_class.num_notes = sizeof(success_notes)/sizeof(success_notes[0]);
    // PLay - that way
    } else if (select == SONG_B) { 
      sound_class.tempo = 120;
      sound_class.melody = that_way_notes;
      sound_class.num_notes = sizeof(that_way_notes)/sizeof(that_way_notes[0]);
    // CUSTOMIZE THIS SOUND FOR YOU
    } else if (select == SONG_CUSTOM) { 
      sound_class.tempo = 120;
      sound_class.melody = custom_song;
      sound_class.num_notes = sizeof(custom_song)/sizeof(custom_song[0]);      
    } else {
      stop_sound();
      Serial.print("ERROR -- you picked an illegal sound\n");
    }

    // Print names
    Serial.print("Playing ");
    print_sound_name(select);
    Serial.print("\n");

}


// ------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------
// Code for timers and piezo PWM setup, using TIMER0 and TIMER1 
// ------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------

// flag for frame rate interrupt
volatile uint8_t int_fired = 0;

// This is called at FRAME_RATE.
ISR(TIMER0_COMPA_vect) {
  int_fired = 1;
}

// Turns on timers
void setup_frame_timers()
{
    uint32_t max_count;
    uint8_t prescalar;
    
    // disable fired flag
    int_fired = 0;
    
    TCCR0A = 0; // set entire TCCR1A register to 0
    TCCR0B = 0; // same for TCCR1B
    TCNT0  = 0; //initialize counter value to 0
    
    // set compare match register for 1hz increments
    max_count = 16e6;
    max_count /= FRAME_RATE;
    max_count /= 64;

    // sets 256x prescalar
    if (max_count >= 256) {
      max_count /= 4;
      max_count -= 1;
      prescalar = _BV(CS02);
    // uses 64x prescalar
    } else { 
      prescalar = _BV(CS01) | _BV(CS00);
    }
    max_count -= 1;
    
    // enable timer
    OCR0A = max_count;
    // turn on CTC mode
    TCCR0A |= (1 << WGM01);
    // Set CS10 and CS12 bits for 1024 prescaler
    TCCR0B |= prescalar;  
    // enable timer compare interrupt
    TIMSK0 |= (1 << OCIE0A);

}




// Sets up the piezo PWM 
void setup_piezo() {

  // setup piezo pin
  //pinMode(piezoPin, OUTPUT);
  pinMode(PIEZO_PIN, INPUT);

  // initialize timer1 
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  OCR1A = 1000;

  // Configure as 16 MHz PWM
  TCCR1A  = _BV(COM1A0) | _BV(WGM11) | _BV(WGM10);    // Fast PWM mode (OCR1A) top
  TCCR1B  = _BV(WGM13) | _BV(WGM12);    // Fast PWM mode (OCR1A) top
  
}

// waits for interrupt to set the flag
void wait_for_frame() {
  while (int_fired == 0) {
  }
  int_fired = 0;
}

// ------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------
// Main Arduino code - setup and loop
// ------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------

// ----------------------------------
// Initialize 
// ---------------------------------
void setup() {
  
  // setupp frame timer and piezo code
  cli();
  setup_piezo();
  setup_frame_timers();
  sei();

  // setup serial port
  Serial.begin(9600);
  
}





// ----------------------------------
// Main loop 
// ---------------------------------
void loop() {
  
  uint8_t sound_count = 0;
  uint32_t count = 0;

  // setts list of songs to play
  // TODO -  Edit your play list here
  uint8_t play_list[] = { SONG_A, SOUND_EFFECT_A, SONG_B }; 
  //uint8_t play_list[] = { SONG_A, SONG_B, YOUR_SONG, SOUND_EFFECT_A, YOUR_SOUND_EFFECT}; 

  Serial.print("Launch the HEYKUBE Sound generator\n");
  Serial.print("Playlist: ");
  for (uint8_t loop1  = 0; loop1 < sizeof(play_list); loop1++) { 
    print_sound_name(play_list[loop1]);
    if (loop1 == (sizeof(play_list)-1)) {
      Serial.print("\n");
    } else {
      Serial.print(", ");
    }
  }
 
  while (true) { 

    // if playing, wait for it to finish
    if (sound_is_playing()) { 
      count = 0;
    // wait for 2 seconds between effects
    } else if (count >= (2*FRAME_RATE)) { 

      // pick sounds
      play_sound(play_list[sound_count]);
      sound_count = (sound_count + 1) & sizeof(play_list);
    }

    // update run and track counts
    sound_run();
    count++;

    // wait for frame interruppt
    wait_for_frame();
    
  }
}
