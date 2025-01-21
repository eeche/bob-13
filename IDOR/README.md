# CTF Project

Dockerfile을 이용해서 이미지를 build하고 run해서 CTF 환경을 구성한다.

```sh
docker build -t idor_challenge .
docker run -d -p 5000:5000 --name idor_container idor_challenge
```

그리고 `http://yourip:5000`에 접속하면 메인페이지를 확인할 수 있다.

로그인/회원가입이 구현되어있다.

로그인을 하면 메인페이지에서 `내 프로필 보기`를 누르면 `/profile`로 이동한다.

이때 관리자 페이지의 profile을 보면 flag가 숨어있다.

<details>
<summary><b>정답</b></summary>

프로필 페이지 url을 자세히 보면 `http://localhost:5000/profile?id=Mw==`와 같이 id값이 있는 것이 수상하다.

이 id값은 문제의 난이도를 높이기 위해서 base64로 인코딩되어 있지만, 눈치가 빠른 사람은 금방 알아차릴 것이다.

심지어 아무 값을 입력하면 친절하게 Invalid Base64-encoded ID 라고 힌트를 준다.

이 id값을 적절히 바꾸면 권한이 없더라도 관리자 페이지의 profile에 접근할 수 있다.

관리자 id=1로 이를 base64로 인코딩한 값은 MQ==이다.

`http://localhost:5000/profile?id=MQ==` 을 입력하면 flag를 얻을 수 있다.
</details>