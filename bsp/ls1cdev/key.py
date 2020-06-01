from machine import Pin
led1 = Pin(("LED1", 52), Pin.OUT_PP)
led2 = Pin(("LED2", 53), Pin.OUT_PP)
key1 = Pin(("KEY1", 85), Pin.IN, Pin.PULL_UP)
key2 = Pin(("KEY2", 86), Pin.IN, Pin.PULL_UP)
while True:
    if key1.value():
        led1.value(1)
    else:
        led1.value(0)
    if key2.value():
        led2.value(1)
    else:
        led2.value(0)