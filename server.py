import socket
import subprocess

def handle_client(client_socket):
    try:
        while True:
            request = client_socket.recv(1024).decode('utf-8')
            if not request:
                break  # Se nenhuma requisição, saia do loop

            print(f"Received request: {request}")  # Log recebimento

            # Mantém os espaços na entrada
            response = subprocess.run(['./gerencia-arquivo.exe'], input=request, text=True, capture_output=True)
            output = response.stdout
            error = response.stderr

            print(f"Executed with input: {request}")
            print("Output:")
            print(output)
            if error:
                print("Error:")
                print(error)

            try:
                if output:
                    client_socket.sendall(output.encode('utf-8'))
                if error:
                    client_socket.sendall(error.encode('utf-8'))
            except socket.error as e:
                print(f"Socket error on sending response: {e}")
                break  # Se erro ao enviar, saia do loop

    except Exception as e:
        print(f"Error executing command: {e}")
        try:
            client_socket.sendall(f"Error executing command: {e}\n".encode('utf-8'))
        except socket.error as e:
            print(f"Socket error on sending exception notice: {e}")

    finally:
        client_socket.close()  # Fechar o socket após sair do loop


def main():
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.bind(('0.0.0.0', 9999))
    server.listen(5)
    print("Server listening on port 9999")
    while True:
        client_socket, addr = server.accept()
        print(f"Accepted connection from {addr}")
        handle_client(client_socket)

if __name__ == "__main__":
    main()
