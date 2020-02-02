from _thread import *
import threading
import socket
import signal
import sys

MESSAGE_SIZE = 1024


def sigint_handler(sig, frame):
    print("Closing server.")
    sys.exit(0)


# decodes the first message from the client containing their name
def get_client_name(client_connection):
    client_name = client_connection.recv(MESSAGE_SIZE)
    client_name = client_name.decode('utf-8')
    return client_name


def listen(client_connection, client_name, chat_thread_lock):
    while True:
        # check to see if parent thread is finished
        if chat_thread_lock.acquire(False):
            print("Listen thread returning")
            break

        # receive message from the client
        client_data = client_connection.recv(MESSAGE_SIZE)
        if not client_data:
            break

        # output clients message to terminal
        print("\n" + client_name + "> " + client_data.decode('utf-8'))

    print("Client has disconnected")


# function that handles the chat loop for the session
def start_chat(client_connection, port, client_name):

    # begin listening thread
    parent_thread = threading.Lock()
    parent_thread.acquire(False);
    start_new_thread(listen, (client_connection, client_name, parent_thread,))

    while True:
        # get user input, prompt with [PORT]>
        user_input = input("\n" + str(port) + "> ")

        # check for '\quit'
        if "\quit" in user_input:
            break

        # combine prompt and message to give to client
        to_send = str(port) + "> " + user_input

    parent_thread.release()


# Function that controls the steps of the function
def serve_client(client_connection, port):
    # get clients name:
    client_name = get_client_name(client_connection)
    print("Connected to: " + client_name)
    start_chat(client_connection, port, client_name)
    print("Closing connection.")
    client_connection.close()


def startup():

    hostname = "localhost"
    port = 12346

    # create a TCP socket connection
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # bind the socket to a port
    sock.bind((hostname, port))
    print("Chat server started on Port: ", port)

    # listen for new connections
    sock.listen(5)
    print("Listening for connections...")

    while True:
        # accept connection
        client, address = sock.accept()

        # lock client thread
        # thread_lock.acquire()
        print("Thread spawned for client: ", address[0], address[1])

        # spawn a new thread
        print("Starting new thread to handle client.")
        start_new_thread(serve_client, (client,port,))

    sock.close()


signal.signal(signal.SIGINT, sigint_handler)
startup()