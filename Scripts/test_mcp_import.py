import socket
import struct
import json

def send_cmd(cmd_type, params):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(('127.0.0.1', 55557))
    msg = json.dumps({'type': cmd_type, 'params': params}).encode('utf-8')
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

res = send_cmd('import_asset', {
    'source_file': 'C:/Users/Admin/Documents/Unreal Projects/WonderTowerTycoon/Assets/WonderWorld/Downtown/Textures/T_Asphalt_Albedo.png',
    'destination_path': '/Game/WonderWorld/Textures/Downtown',
    'destination_name': 'T_Asphalt_Albedo'
})
print(json.dumps(res, indent=2))
