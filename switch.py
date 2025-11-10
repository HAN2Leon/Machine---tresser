import RPi.GPIO as GPIO # type: ignore
import time
import spidev, math # type: ignore

time_wait_MIN = 0.2
time_wait_MAX = 3
DEADZONE = 0.03

# ==== Définition des broches GPIO (numérotation BCM) ====
PUL = 14    # Broche connectée à PUL- du driver DM542T
DIR = 15    # Broche connectée à DIR- du driver DM542T

# ==== Paramètres du moteur ====
MICROSTEP = 3200          # Rapport de micro-pas choisi sur le driver (1 = plein pas)
HALF_TURN_STEPS = MICROSTEP // 2               # Demi-tour = 100 impulsions en plein pas

# ==== Temporisation du signal (détermine la vitesse) ====
PULSE = 0.000008   # Durée du niveau haut (8µs)
# minimum = 0.000005


# ==== Initialisation du GPIO ====
GPIO.setmode(GPIO.BCM)      # Utiliser la numérotation BCM des broches
GPIO.setup(PUL, GPIO.OUT)   # Déclarer la broche PUL en sortie
GPIO.setup(DIR, GPIO.OUT)   # Déclarer la broche DIR en sortie

# ==== Définir le sens de rotation ====
GPIO.output(DIR, GPIO.HIGH)   # HIGH ou LOW selon le sens désiré

spi = spidev.SpiDev()
spi.open(0,0)             # SPI CE0
spi.max_speed_hz = 100000
spi.mode = 0

def read_adc(ch=0):
    r = spi.xfer2([1, (0x80 | (ch << 4)) & 0xFF, 0])
    print(r)
    val = ((r[1] & 3) << 8) | r[2]
    return val
    
def log_map(val, vmin=0, vmax=1023):
    x = max(0.0, min(1.0, (val - vmin) / (vmax - vmin)))
    if x < DEADZONE:
        return 0.0
    x = (x - DEADZONE) / (1.0 - DEADZONE)
    return  time_wait_MIN* ((time_wait_MAX / time_wait_MIN) ** x)

raw = 0
time_wait = 0
# time_wait = 0.2
#temps attente entre 2 rotation
try:
    while True:

        # --- Génération des impulsions pour un demi-tour ---
        print("TOUR :", HALF_TURN_STEPS)
        for _ in range(HALF_TURN_STEPS):
            GPIO.output(PUL, GPIO.HIGH)   # front montant
            time.sleep(PULSE)
            GPIO.output(PUL, GPIO.LOW)    # front descendant
            time.sleep(PULSE)
            
        raw = read_adc(0)
        print(raw)
        time_wait = log_map(raw)
        time.sleep(time_wait)
        
        print("Temps de rotation :", PULSE*HALF_TURN_STEPS*2, "Time wait : ", time_wait)
        
except KeyboardInterrupt:
    print("Programme interrompu par l’utilisateur.")

finally:
    GPIO.cleanup()  # Réinitialise toutes les broches GPIO