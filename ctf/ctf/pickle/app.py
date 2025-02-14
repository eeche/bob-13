import os
import base64
import pickle
from flask import Flask, request, render_template

app = Flask(__name__)

# 오프라인 환경에서도 동작 가능하도록, flag.txt를 같은 폴더에 두고 open해서 사용할 수 있음.
FLAG_FILE = "flag.txt"

@app.route("/")
def index():
    return render_template("index.html")

@app.route("/load_profile")
def load_profile():
    """
    역직렬화 취약점:
    - 사용자가 ?data=Base64Encoded 로 전달하면,
    - 서버가 그 데이터를 pickle.loads()로 역직렬화 -> 임의 코드 실행 가능
    """
    data = request.args.get("data", "")
    if not data:
        return "No data provided. Try ?data=Base64_of_your_pickle", 400

    try:
        bin_data = base64.b64decode(data)
        obj = pickle.loads(bin_data)  # <-- 취약점 발생 지점
        return f"Profile loaded: {obj}"
    except Exception as e:
        return f"Error during pickle.loads(): {e}", 500

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
