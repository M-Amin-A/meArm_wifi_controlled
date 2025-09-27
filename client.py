import keyboard
import time
import socket

DEBUG = False

udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
server_address = ('192.168.113.101', 8888)

def read_keys(pos_key, neg_key):
    if keyboard.is_pressed(pos_key) and not keyboard.is_pressed(neg_key):
        return 1
    elif not keyboard.is_pressed(pos_key) and keyboard.is_pressed(neg_key):
        return -1
    return 0


try:
    while True:
        if keyboard.is_pressed('esc'):
            break

        baseInput = 5 * read_keys('a', 'd')
        rInput = 5 * read_keys('w', 's')
        zInput = 5 * read_keys('i', 'k')
        gripperInput = 5 * read_keys('l', 'j')

        message = f"{baseInput},{rInput},{zInput},{gripperInput}"
        udp_socket.sendto(message.encode(), server_address)
        if DEBUG:
            print(f"Sent: {message}")

        time.sleep(0.02)
finally:
    udp_socket.close()
