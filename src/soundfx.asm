INCLUDE "sdk/hardware.inc"

; http://www.devrs.com/gb/files/sndtab.html
DEF NOTE_C_3 EQU 44
DEF NOTE_Cs3 EQU 156
DEF NOTE_D_3 EQU 262
DEF NOTE_Ds3 EQU 363
DEF NOTE_E_3 EQU 457
DEF NOTE_F_3 EQU 547
DEF NOTE_Fs3 EQU 631
DEF NOTE_G_3 EQU 710
DEF NOTE_Gs3 EQU 786
DEF NOTE_A_3 EQU 854
DEF NOTE_As3 EQU 923
DEF NOTE_B_3 EQU 986
DEF NOTE_C_4 EQU 1046
DEF NOTE_Cs4 EQU 1102
DEF NOTE_D_4 EQU 1155
DEF NOTE_Ds4 EQU 1205
DEF NOTE_E_4 EQU 1253
DEF NOTE_F_4 EQU 1297
DEF NOTE_Fs4 EQU 1339
DEF NOTE_G_4 EQU 1379
DEF NOTE_Gs4 EQU 1417
DEF NOTE_A_4 EQU 1452
DEF NOTE_As4 EQU 1486
DEF NOTE_B_4 EQU 1517
DEF NOTE_C_5 EQU 1546
DEF NOTE_Cs5 EQU 1575
DEF NOTE_D_5 EQU 1602
DEF NOTE_Ds5 EQU 1627
DEF NOTE_E_5 EQU 1650
DEF NOTE_F_5 EQU 1673
DEF NOTE_Fs5 EQU 1694
DEF NOTE_G_5 EQU 1714
DEF NOTE_Gs5 EQU 1732
DEF NOTE_A_5 EQU 1750
DEF NOTE_As5 EQU 1767
DEF NOTE_B_5 EQU 1783
DEF NOTE_C_6 EQU 1798
DEF NOTE_Cs6 EQU 1812
DEF NOTE_D_6 EQU 1825
DEF NOTE_Ds6 EQU 1837
DEF NOTE_E_6 EQU 1849
DEF NOTE_F_6 EQU 1860
DEF NOTE_Fs6 EQU 1871
DEF NOTE_G_6 EQU 1881
DEF NOTE_Gs6 EQU 1890
DEF NOTE_A_6 EQU 1899
DEF NOTE_As6 EQU 1907
DEF NOTE_B_6 EQU 1915
DEF NOTE_C_7 EQU 1923
DEF NOTE_Cs7 EQU 1930
DEF NOTE_D_7 EQU 1936
DEF NOTE_Ds7 EQU 1943
DEF NOTE_E_7 EQU 1949
DEF NOTE_F_7 EQU 1954
DEF NOTE_Fs7 EQU 1959
DEF NOTE_G_7 EQU 1964
DEF NOTE_Gs7 EQU 1969
DEF NOTE_A_7 EQU 1974
DEF NOTE_As7 EQU 1978
DEF NOTE_B_7 EQU 1982
DEF NOTE_C_8 EQU 1985
DEF NOTE_Cs8 EQU 1988
DEF NOTE_D_8 EQU 1992
DEF NOTE_Ds8 EQU 1995
DEF NOTE_E_8 EQU 1998
DEF NOTE_F_8 EQU 2001
DEF NOTE_Fs8 EQU 2004
DEF NOTE_G_8 EQU 2006
DEF NOTE_Gs8 EQU 2009
DEF NOTE_A_8 EQU 2011
DEF NOTE_As8 EQU 2013
DEF NOTE_B_8 EQU 2015

DEF SOUND_END EQU $FF
DEF CHANGE_PRIORITY EQU $FE
DEF PAN_CH1 EQU $FD
DEF PAN_CH2 EQU $FC
DEF PAN_CH3 EQU $FB
DEF PAN_CH4 EQU $FA
DEF SET_MUSIC_MUTE EQU $F9

DEF MUTE_CH1 EQU 0 << 4
DEF MUTE_CH2 EQU 1 << 4
DEF MUTE_CH3 EQU 2 << 4
DEF MUTE_CH4 EQU 3 << 4
DEF MUSIC_MUTE EQU 1
DEF MUSIC_UNMUTE EQU 0

DEF AUDHIGH_NO_RESTART EQU 0 ; missing from hardware.inc (counterpart to AUDHIGH_RESTART)

; --- Channel 2 ---

; \1 = Duty (use the AUDLEN_DUTY definitions from hardware.inc)
; \2 = Sound Length (0-63)
; \3 = Frame Wait (0-255)
MACRO CH2_LENGTH_DUTY
    DB LOW(rNR21), \1 | \2, \3
ENDM

; \1 = Initial Volume (0-$F)
; \2 = Envelope Direction (use the AUDENV definitions from hardware.inc)
; \3 = Number of envelope sweep (0-7)
; \4 = Frame Wait (0-255)
MACRO CH2_VOLENV
    DB LOW(rNR22), (\1 << 4) | \2 | \3, \4
ENDM

; \1 = Frequency (use the NOTE definitions)
; \2 = Stop sound after length expires (use the AUDHIGH_LENGTH defs from hardware.inc)
; \3 = If sound should be restarted (use the AUDHIGH defs)
; \4 = Number of frames to wait after effect (0-255)
MACRO CH2_FREQ
    DB LOW(rNR23), LOW(\1), 0
    DB LOW(rNR24), HIGH(\1) | \2 | \3, \4
ENDM

; --- Channel 4 ---

; \1 = Sound Length (0-63)
; \2 = Frame Wait (0-255)
MACRO CH4_LENGTH
    DB LOW(rNR41), \1, \2
ENDM

; \1 = Initial Volume (0-$F)
; \2 = Envelope Direction (use the AUDENV definitions from hardware.inc)
; \3 = Number of envelope sweep (0-7)
; \4 = Frame Wait (0-255)
MACRO CH4_VOLENV
    DB LOW(rNR42), (\1 << 4) | \2 | \3, \4
ENDM

; \1 = Counter Width (use the AUD4POLY defs)
; \2 = Shift Clock Frequency (0-15)
; \3 = Frequency Dividing Ratio (0-7)
; \4 = Frame Wait (0-255)
MACRO CH4_POLYCT
    DB LOW(rNR43), \1 | (\2 << 4) | \3, \4
ENDM

; \1 = Stop sound after length expires (use the AUDHIGH_LENGTH defs from hardware.inc)
; \2 = If sound should be restarted (use the AUDHIGH defs)
; \3 = Frame Wait (0-255)
MACRO CH4_RESTART
    DB LOW(rNR44), \1 | \2, \3
ENDM

SECTION "Sound FX", ROM0

; https://daid.github.io/gbsfx-studio/

FX_ShortCrash:: ; (CH4) Played for small "bumps" between cars
    DB 1 ; FX channel
    DB 0 ; Starting priority byte
    DB SET_MUSIC_MUTE, MUTE_CH4 | MUSIC_MUTE, 0
    DB PAN_CH4, AUDTERM_4_LEFT | AUDTERM_4_RIGHT, 0
    CH4_VOLENV $6, AUDENV_DOWN, 1, 0
    CH4_POLYCT AUD4POLY_15STEP, $8, 0, 0
    CH4_RESTART AUDHIGH_LENGTH_OFF, AUDHIGH_RESTART, 5
    DB SET_MUSIC_MUTE, MUTE_CH4 | MUSIC_UNMUTE, 0
    DB SOUND_END

FX_CarExplode:: ; (CH4) Played for car explosions
    DB 1 ; FX channel
    DB 1 ; Starting priority byte
    DB SET_MUSIC_MUTE, MUTE_CH4 | MUSIC_MUTE, 0
    DB PAN_CH4, AUDTERM_4_LEFT | AUDTERM_4_RIGHT, 0
    CH4_VOLENV $D, AUDENV_DOWN, 2, 0
    CH4_POLYCT AUD4POLY_15STEP, $9, 0, 0
    CH4_RESTART AUDHIGH_LENGTH_OFF, AUDHIGH_RESTART, 20
    DB SET_MUSIC_MUTE, MUTE_CH4 | MUSIC_UNMUTE, 0
    DB SOUND_END

FX_PlayerMissile:: ; (CH4) When player fires a missile
    DB 1 ; FX channel
    DB 1 ; Starting priority byte
    DB SET_MUSIC_MUTE, MUTE_CH4 | MUSIC_MUTE, 0
    DB PAN_CH4, AUDTERM_4_LEFT | AUDTERM_4_RIGHT, 0
    CH4_VOLENV $C, AUDENV_DOWN, 3, 0
    CH4_RESTART AUDHIGH_LENGTH_OFF, AUDHIGH_RESTART, 0
    CH4_POLYCT AUD4POLY_15STEP, $4, 0, 1
    CH4_POLYCT AUD4POLY_15STEP, $5, 0, 1
    CH4_POLYCT AUD4POLY_15STEP, $6, 1, 29
    DB SET_MUSIC_MUTE, MUTE_CH4 | MUSIC_UNMUTE, 0
    DB SOUND_END

FX_EnemyMissile:: ; (CH4) When an enemy fires a missile
    DB 1 ; FX channel
    DB 1 ; Starting priority byte
    DB SET_MUSIC_MUTE, MUTE_CH4 | MUSIC_MUTE, 0
    DB PAN_CH4, AUDTERM_4_LEFT | AUDTERM_4_RIGHT, 0
    CH4_VOLENV $C, AUDENV_DOWN, 3, 0
    CH4_RESTART AUDHIGH_LENGTH_OFF, AUDHIGH_RESTART, 0
    CH4_POLYCT AUD4POLY_15STEP, $4, 0, 1
    CH4_POLYCT AUD4POLY_15STEP, $5, 0, 1
    CH4_POLYCT AUD4POLY_15STEP, $6, 2, 29
    DB SET_MUSIC_MUTE, MUTE_CH4 | MUSIC_UNMUTE, 0
    DB SOUND_END

FX_HeliExplode:: ; (CH4) Played for helicopter explosions
    DB 1 ; FX channel
    DB 2 ; Starting priority byte
    DB SET_MUSIC_MUTE, MUTE_CH4 | MUSIC_MUTE, 0
    DB PAN_CH4, AUDTERM_4_LEFT | AUDTERM_4_RIGHT, 0
    CH4_VOLENV $E, AUDENV_DOWN, 3, 0
    CH4_RESTART AUDHIGH_LENGTH_OFF, AUDHIGH_RESTART, 0
    CH4_POLYCT AUD4POLY_15STEP, $4, 1, 1
    CH4_POLYCT AUD4POLY_15STEP, $6, 1, 1
    CH4_POLYCT AUD4POLY_15STEP, $7, 2, 35
    DB SET_MUSIC_MUTE, MUTE_CH4 | MUSIC_UNMUTE, 0
    DB SOUND_END

FX_TruckDropRock:: ; (CH4) Played when the truck uses it's special to drop a rock
    DB 1 ; FX channel
    DB 1 ; Starting priority byte
    DB SET_MUSIC_MUTE, MUTE_CH4 | MUSIC_MUTE, 0
    DB PAN_CH4, AUDTERM_4_LEFT | AUDTERM_4_RIGHT, 0
    CH4_VOLENV $D, AUDENV_DOWN, 2, 0
    CH4_RESTART AUDHIGH_LENGTH_OFF, AUDHIGH_RESTART, 0
    CH4_POLYCT AUD4POLY_15STEP, $4, 3, 20
    DB SET_MUSIC_MUTE, MUTE_CH4 | MUSIC_UNMUTE, 0
    DB SOUND_END

FX_TimeCarBlip:: ; (CH4) Played when the time car activates it's special
    DB 1 ; FX channel
    DB 1 ; Starting priority byte
    DB SET_MUSIC_MUTE, MUTE_CH4 | MUSIC_MUTE, 0
    DB PAN_CH4, AUDTERM_4_LEFT | AUDTERM_4_RIGHT, 0
    CH4_VOLENV $C, AUDENV_DOWN, 3, 0
    CH4_POLYCT AUD4POLY_7STEP, $2, 7, 0
    CH4_RESTART AUDHIGH_LENGTH_OFF, AUDHIGH_RESTART, 2
    CH4_POLYCT AUD4POLY_15STEP, $1, 6, 0
    CH4_RESTART AUDHIGH_LENGTH_OFF, AUDHIGH_RESTART, 20
    DB SET_MUSIC_MUTE, MUTE_CH4 | MUSIC_UNMUTE, 0
    DB SOUND_END

FX_WarningBeep:: ; (CH2) Played when the warning sign flashes
    DB 0 ; FX channel
    DB 0 ; Starting priority byte
    DB SET_MUSIC_MUTE, MUTE_CH2 | MUSIC_MUTE, 0
    DB PAN_CH2, AUDTERM_2_LEFT | AUDTERM_2_RIGHT, 0
    CH2_LENGTH_DUTY AUDLEN_DUTY_50, 0, 0
    CH2_VOLENV $E, AUDENV_DOWN, 1, 0
    CH2_FREQ NOTE_D_4, AUDHIGH_LENGTH_OFF, AUDHIGH_RESTART, 7
    DB SET_MUSIC_MUTE, MUTE_CH2 | MUSIC_UNMUTE, 0
    DB SOUND_END

FX_PlayerMissileCharged:: ; (CH2) Played when the player's missile bar is fully charged
    DB 0 ; FX channel
    DB 1 ; Starting priority byte
    DB SET_MUSIC_MUTE, MUTE_CH2 | MUSIC_MUTE, 0
    DB PAN_CH2, AUDTERM_2_LEFT | AUDTERM_2_RIGHT, 0
    CH2_LENGTH_DUTY AUDLEN_DUTY_50, 0, 0
    CH2_VOLENV $7, AUDENV_DOWN, 1, 0
    CH2_FREQ NOTE_Cs5, AUDHIGH_LENGTH_OFF, AUDHIGH_RESTART, 5
    CH2_FREQ NOTE_F_5, AUDHIGH_LENGTH_OFF, AUDHIGH_RESTART, 5
    DB SET_MUSIC_MUTE, MUTE_CH2 | MUSIC_UNMUTE, 0
    DB SOUND_END

FX_PlayerSpecialCharged:: ; (CH2) Played when the player's special bar is fully charged
    DB 0 ; FX channel
    DB 1 ; Starting priority byte
    DB SET_MUSIC_MUTE, MUTE_CH2 | MUSIC_MUTE, 0
    DB PAN_CH2, AUDTERM_2_LEFT | AUDTERM_2_RIGHT, 0
    CH2_LENGTH_DUTY AUDLEN_DUTY_50, 0, 0
    CH2_VOLENV $7, AUDENV_DOWN, 1, 0
    CH2_FREQ NOTE_Ds5, AUDHIGH_LENGTH_OFF, AUDHIGH_RESTART, 5
    CH2_FREQ NOTE_G_5, AUDHIGH_LENGTH_OFF, AUDHIGH_RESTART, 5
    DB SET_MUSIC_MUTE, MUTE_CH2 | MUSIC_UNMUTE, 0
    DB SOUND_END

_FX_MenuBip:: ; (CH2) The small bip when moving between menu items
    DB 0 ; Starting priority byte
    DB PAN_CH2, AUDTERM_2_LEFT | AUDTERM_2_RIGHT, 0
    CH2_LENGTH_DUTY AUDLEN_DUTY_12_5, 0, 0
    CH2_VOLENV $3, AUDENV_DOWN, 2, 0
    CH2_FREQ NOTE_A_3, AUDHIGH_LENGTH_OFF, AUDHIGH_RESTART, 5
    DB SOUND_END

FX_CarSelect:: ; (CH2) When a car is selected in the garage
    DB 0 ; FX channel
    DB 0 ; Starting priority byte
    DB SET_MUSIC_MUTE, MUTE_CH2 | MUSIC_MUTE, 0
    DB PAN_CH2, AUDTERM_2_LEFT | AUDTERM_2_RIGHT, 0
    CH2_LENGTH_DUTY AUDLEN_DUTY_50, 0, 0
    CH2_VOLENV $A, AUDENV_DOWN, 1, 0
    CH2_FREQ NOTE_D_4, AUDHIGH_LENGTH_OFF, AUDHIGH_RESTART, 5
    CH2_FREQ NOTE_E_4, AUDHIGH_LENGTH_OFF, AUDHIGH_RESTART, 6
    DB SET_MUSIC_MUTE, MUTE_CH2 | MUSIC_UNMUTE, 0
    DB SOUND_END

FX_CarBuy:: ; (CH2) When a car is bought / upgraded in the garage
    DB 0 ; FX channel
    DB 0 ; Starting priority byte
    DB SET_MUSIC_MUTE, MUTE_CH2 | MUSIC_MUTE, 0
    DB PAN_CH2, AUDTERM_2_LEFT | AUDTERM_2_RIGHT, 0
    CH2_LENGTH_DUTY AUDLEN_DUTY_50, 0, 0
    CH2_VOLENV $A, AUDENV_DOWN, 2, 0
    CH2_FREQ NOTE_D_5, AUDHIGH_LENGTH_OFF, AUDHIGH_RESTART, 2
    CH2_FREQ NOTE_Fs5, AUDHIGH_LENGTH_OFF, AUDHIGH_NO_RESTART, 2
    CH2_FREQ NOTE_A_5, AUDHIGH_LENGTH_OFF, AUDHIGH_NO_RESTART, 2
    CH2_FREQ NOTE_D_5, AUDHIGH_LENGTH_OFF, AUDHIGH_NO_RESTART, 2
    CH2_FREQ NOTE_Fs5, AUDHIGH_LENGTH_OFF, AUDHIGH_NO_RESTART, 2
    CH2_FREQ NOTE_A_5, AUDHIGH_LENGTH_OFF, AUDHIGH_NO_RESTART, 2
    CH2_FREQ NOTE_D_5, AUDHIGH_LENGTH_OFF, AUDHIGH_NO_RESTART, 2
    CH2_FREQ NOTE_Fs5, AUDHIGH_LENGTH_OFF, AUDHIGH_NO_RESTART, 2
    CH2_FREQ NOTE_A_5, AUDHIGH_LENGTH_OFF, AUDHIGH_NO_RESTART, 2
    CH2_FREQ NOTE_D_5, AUDHIGH_LENGTH_OFF, AUDHIGH_NO_RESTART, 2
    CH2_FREQ NOTE_Fs5, AUDHIGH_LENGTH_OFF, AUDHIGH_NO_RESTART, 2
    CH2_FREQ NOTE_A_5, AUDHIGH_LENGTH_OFF, AUDHIGH_NO_RESTART, 2
    DB SET_MUSIC_MUTE, MUTE_CH2 | MUSIC_UNMUTE, 0
    DB SOUND_END

FX_CarFailBuy:: ; (CH2) When you try to buy / upgrade a car but don't have enough money
    DB 0 ; FX channel
    DB 0 ; Starting priority byte
    DB SET_MUSIC_MUTE, MUTE_CH2 | MUSIC_MUTE, 0
    DB PAN_CH2, AUDTERM_2_LEFT | AUDTERM_2_RIGHT, 0
    CH2_LENGTH_DUTY AUDLEN_DUTY_50, 0, 0
    CH2_VOLENV $A, AUDENV_DOWN, 2, 0
    CH2_FREQ NOTE_Ds4, AUDHIGH_LENGTH_OFF, AUDHIGH_RESTART, 2
    CH2_FREQ NOTE_Fs4, AUDHIGH_LENGTH_OFF, AUDHIGH_NO_RESTART, 2
    CH2_FREQ NOTE_As4, AUDHIGH_LENGTH_OFF, AUDHIGH_NO_RESTART, 2
    CH2_FREQ NOTE_Ds4, AUDHIGH_LENGTH_OFF, AUDHIGH_NO_RESTART, 2
    CH2_FREQ NOTE_Fs4, AUDHIGH_LENGTH_OFF, AUDHIGH_NO_RESTART, 2
    CH2_FREQ NOTE_As4, AUDHIGH_LENGTH_OFF, AUDHIGH_NO_RESTART, 2
    CH2_FREQ NOTE_Ds4, AUDHIGH_LENGTH_OFF, AUDHIGH_NO_RESTART, 2
    CH2_FREQ NOTE_Fs4, AUDHIGH_LENGTH_OFF, AUDHIGH_NO_RESTART, 2
    CH2_FREQ NOTE_As4, AUDHIGH_LENGTH_OFF, AUDHIGH_NO_RESTART, 2
    CH2_FREQ NOTE_Ds4, AUDHIGH_LENGTH_OFF, AUDHIGH_NO_RESTART, 2
    CH2_FREQ NOTE_Fs4, AUDHIGH_LENGTH_OFF, AUDHIGH_NO_RESTART, 2
    CH2_FREQ NOTE_As4, AUDHIGH_LENGTH_OFF, AUDHIGH_NO_RESTART, 2
    DB SET_MUSIC_MUTE, MUTE_CH2 | MUSIC_UNMUTE, 0
    DB SOUND_END

FX_Pause:: ; (CH2) Played when the pause menu is opened
    DB 0 ; FX channel
    DB 0 ; Starting priority byte
    DB SET_MUSIC_MUTE, MUTE_CH2 | MUSIC_MUTE, 0
    DB PAN_CH2, AUDTERM_2_LEFT | AUDTERM_2_RIGHT, 0
    CH2_LENGTH_DUTY AUDLEN_DUTY_50, 0, 0
    CH2_VOLENV $F, AUDENV_DOWN, 1, 0
    CH2_FREQ NOTE_C_3, AUDHIGH_LENGTH_OFF, AUDHIGH_RESTART, 3
    CH2_FREQ NOTE_D_3, AUDHIGH_LENGTH_OFF, AUDHIGH_RESTART, 3
    CH2_FREQ NOTE_F_3, AUDHIGH_LENGTH_OFF, AUDHIGH_RESTART, 4
    DB SET_MUSIC_MUTE, MUTE_CH2 | MUSIC_UNMUTE, 0
    DB SOUND_END

FX_Unpause:: ; (CH2) Played when the pause menu is closed
    DB 0 ; FX channel
    DB 0 ; Starting priority byte
    DB SET_MUSIC_MUTE, MUTE_CH2 | MUSIC_MUTE, 0
    DB PAN_CH2, AUDTERM_2_LEFT | AUDTERM_2_RIGHT, 0
    CH2_LENGTH_DUTY AUDLEN_DUTY_50, 0, 0
    CH2_VOLENV $F, AUDENV_DOWN, 1, 0
    CH2_FREQ NOTE_F_3, AUDHIGH_LENGTH_OFF, AUDHIGH_RESTART, 3
    CH2_FREQ NOTE_D_3, AUDHIGH_LENGTH_OFF, AUDHIGH_RESTART, 3
    CH2_FREQ NOTE_C_3, AUDHIGH_LENGTH_OFF, AUDHIGH_RESTART, 4
    DB SET_MUSIC_MUTE, MUTE_CH2 | MUSIC_UNMUTE, 0
    DB SOUND_END

FX_TitleScreenStart:: ; (CH2) Played when pressing "Start" on the title screen
    DB 0 ; FX channel
    DB 0 ; Starting priority byte
    DB SET_MUSIC_MUTE, MUTE_CH2 | MUSIC_MUTE, 0
    DB PAN_CH2, AUDTERM_2_LEFT | AUDTERM_2_RIGHT, 0
    CH2_LENGTH_DUTY AUDLEN_DUTY_75, 0, 0
    CH2_VOLENV $F, AUDENV_DOWN, 0, 0
    CH2_FREQ NOTE_D_5, AUDHIGH_LENGTH_OFF, AUDHIGH_RESTART, 3
    CH2_VOLENV $E, AUDENV_DOWN, 0, 0
    CH2_FREQ NOTE_Fs5, AUDHIGH_LENGTH_OFF, AUDHIGH_RESTART, 3
    CH2_VOLENV $D, AUDENV_DOWN, 0, 0
    CH2_FREQ NOTE_A_5, AUDHIGH_LENGTH_OFF, AUDHIGH_RESTART, 3
    CH2_VOLENV $C, AUDENV_DOWN, 0, 0
    CH2_FREQ NOTE_D_5, AUDHIGH_LENGTH_OFF, AUDHIGH_RESTART, 3
    CH2_VOLENV $B, AUDENV_DOWN, 0, 0
    CH2_FREQ NOTE_Fs5, AUDHIGH_LENGTH_OFF, AUDHIGH_RESTART, 3
    CH2_VOLENV $A, AUDENV_DOWN, 0, 0
    CH2_FREQ NOTE_A_5, AUDHIGH_LENGTH_OFF, AUDHIGH_RESTART, 3
    CH2_VOLENV $9, AUDENV_DOWN, 0, 0
    CH2_FREQ NOTE_D_5, AUDHIGH_LENGTH_OFF, AUDHIGH_RESTART, 3
    CH2_VOLENV $8, AUDENV_DOWN, 0, 0
    CH2_FREQ NOTE_Fs5, AUDHIGH_LENGTH_OFF, AUDHIGH_RESTART, 3
    CH2_VOLENV $7, AUDENV_DOWN, 0, 0
    CH2_FREQ NOTE_A_5, AUDHIGH_LENGTH_OFF, AUDHIGH_RESTART, 3
    CH2_VOLENV $6, AUDENV_DOWN, 0, 0
    CH2_FREQ NOTE_D_5, AUDHIGH_LENGTH_OFF, AUDHIGH_RESTART, 3
    CH2_VOLENV $5, AUDENV_DOWN, 0, 0
    CH2_FREQ NOTE_Fs5, AUDHIGH_LENGTH_OFF, AUDHIGH_RESTART, 3
    CH2_VOLENV $4, AUDENV_DOWN, 0, 0
    CH2_FREQ NOTE_A_5, AUDHIGH_LENGTH_OFF, AUDHIGH_RESTART, 3
    CH2_VOLENV $3, AUDENV_DOWN, 0, 0
    CH2_FREQ NOTE_D_5, AUDHIGH_LENGTH_OFF, AUDHIGH_RESTART, 3
    CH2_VOLENV $2, AUDENV_DOWN, 0, 0
    CH2_FREQ NOTE_Fs5, AUDHIGH_LENGTH_OFF, AUDHIGH_RESTART, 3
    CH2_VOLENV $1, AUDENV_DOWN, 2, 0
    CH2_FREQ NOTE_A_5, AUDHIGH_LENGTH_OFF, AUDHIGH_RESTART, 3
    DB SET_MUSIC_MUTE, MUTE_CH2 | MUSIC_UNMUTE, 0
    DB SOUND_END