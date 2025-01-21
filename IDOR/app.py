import os
import sqlite3
import base64
from flask import Flask, request, session, redirect, url_for, render_template
from werkzeug.security import generate_password_hash, check_password_hash

app = Flask(__name__)
app.secret_key = "super_secret_key_for_sessions"  # 실제 프로덕션에서는 안전하게 관리

DB_PATH = os.path.join(os.path.dirname(__file__), "idor.db")

@app.template_filter('b64encode')
def b64encode_filter(value):
    import base64
    return base64.b64encode(str(value).encode()).decode()

def get_db_connection():
    """SQLite DB 커넥션을 반환하는 헬퍼 함수."""
    conn = sqlite3.connect(DB_PATH)
    # row를 dict처럼 사용하기 위해
    conn.row_factory = sqlite3.Row
    return conn

def init_db():
    """DB 초기화: users 테이블이 없으면 생성, 기본 admin 유저 삽입."""
    with get_db_connection() as conn:
        cur = conn.cursor()
        # users 테이블 없으면 생성
        cur.execute("""
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE NOT NULL,
            password_hash TEXT NOT NULL,
            profile TEXT,
            is_admin INTEGER DEFAULT 0
        )
        """)
        conn.commit()

        # 기본 관리자 유저가 있는지 확인
        cur.execute("SELECT id FROM users WHERE username = ?", ("admin",))
        row = cur.fetchone()
        if row is None:
            # 없으면 새로 삽입
            admin_pass_hash = generate_password_hash("bpfisg0d!")
            admin_profile = """관리자 프로필<br>
                               flag{id0r_ch@llenge_fl@g}"""
            cur.execute("""
                INSERT INTO users (username, password_hash, profile, is_admin)
                VALUES (?, ?, ?, ?)
            """, ("admin", admin_pass_hash, admin_profile, 1))
            conn.commit()

        # 기본 예시 유저 alice 있는지 확인
        cur.execute("SELECT id FROM users WHERE username = ?", ("alice",))
        row = cur.fetchone()
        if row is None:
            alice_pass_hash = generate_password_hash("bpfisg0d!")
            alice_profile = "Alice의 프로필 정보"
            cur.execute("""
                INSERT INTO users (username, password_hash, profile, is_admin)
                VALUES (?, ?, ?, ?)
            """, ("alice", alice_pass_hash, alice_profile, 0))
            conn.commit()

@app.route("/")
def index():
    user_id = session.get("user_id")
    username = None

    if user_id:
        # DB에서 username 조회
        with get_db_connection() as conn:
            cur = conn.cursor()
            cur.execute("SELECT username FROM users WHERE id = ?", (user_id,))
            row = cur.fetchone()
            if row:
                username = row["username"]
    
    return render_template("index.html", username=username)

@app.route("/register", methods=["GET", "POST"])
def register():
    if request.method == "POST":
        username = request.form.get("username")
        password = request.form.get("password")
        if not username or not password:
            return "필수 입력값이 누락되었습니다.", 400

        with get_db_connection() as conn:
            cur = conn.cursor()
            # username 중복 체크
            cur.execute("SELECT id FROM users WHERE username = ?", (username,))
            row = cur.fetchone()
            if row:
                return "이미 존재하는 사용자명입니다.", 400

            # 새 유저 삽입
            pass_hash = generate_password_hash(password)
            cur.execute("""
                INSERT INTO users (username, password_hash, profile, is_admin)
                VALUES (?, ?, ?, ?)
            """, (username, pass_hash, f"{username}의 기본 프로필 정보", 0))
            conn.commit()

        return redirect(url_for("login"))

    return render_template("register.html")

@app.route("/login", methods=["GET", "POST"])
def login():
    if request.method == "POST":
        username = request.form.get("username")
        password = request.form.get("password")

        if not username or not password:
            return "필수 입력값이 누락되었습니다.", 400

        with get_db_connection() as conn:
            cur = conn.cursor()
            cur.execute("SELECT id, password_hash FROM users WHERE username = ?", (username,))
            row = cur.fetchone()
            if not row:
                return "존재하지 않는 사용자명입니다.", 400
            
            user_id = row["id"]
            password_hash_db = row["password_hash"]

            if check_password_hash(password_hash_db, password):
                session["user_id"] = user_id
                return redirect(url_for("index"))
            else:
                return "패스워드가 올바르지 않습니다.", 400

    return render_template("login.html")

@app.route("/logout")
def logout():
    session.clear()
    return redirect(url_for("index"))

@app.route("/profile")
def profile():
    """
    IDOR 취약점 발생: user_id를 Base64로 인코딩했지만, 권한 체크는 여전히 누락되어 있어,
    다른 계정(관리자)의 Base64 값만 알면 접근 가능!
    """
    encoded_id = request.args.get("id")
    if not encoded_id:
        return "No 'id' parameter provided.", 400
    
    try:
        # Base64 디코딩: 'MTIz' -> b'123' -> "123"
        decoded_bytes = base64.b64decode(encoded_id)
        uid = int(decoded_bytes.decode("utf-8"))
    except Exception:
        return "Invalid Base64-encoded ID.", 400
    
    with get_db_connection() as conn:
        cur = conn.cursor()
        cur.execute("SELECT username, profile FROM users WHERE id = ?", (uid,))
        row = cur.fetchone()
        if not row:
            return "존재하지 않는 사용자 ID입니다.", 404
        
        # 여전히 "session['user_id'] == uid" 비교 등이 없으므로 IDOR 가능!
        username = row["username"]
        user_profile = row["profile"]

    return render_template("profile.html", username=username, user_profile=user_profile)


if __name__ == "__main__":
    # 서버 실행 전 DB 초기화
    init_db()
    app.run(host="0.0.0.0", port=5000)
