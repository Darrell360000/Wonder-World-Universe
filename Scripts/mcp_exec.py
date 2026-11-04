import socket
import struct
import json
import sys

def run_unreal_code(code: str):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(('127.0.0.1', 55557))
    msg = json.dumps({'type': 'execute_python', 'params': {'code': code}}).encode('utf-8')
    s.sendall(struct.pack('>I', len(msg)) + msg)
    raw_len = s.recv(4)
    resp_len = struct.unpack('>I', raw_len)[0]
    data = b''
    while len(data) < resp_len:
        chunk = s.recv(min(resp_len - len(data), 65536))
        if not chunk:
            break
        data += chunk
    s.close()
    return json.loads(data.decode('utf-8'))

if __name__ == '__main__':
    if len(sys.argv) > 1:
        with open(sys.argv[1], 'r', encoding='utf-8') as f:
            code = f.read()
    else:
        code = sys.stdin.read()
    res = run_unreal_code(code)
    print(json.dumps(res, indent=2))
