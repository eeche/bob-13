import requests
import json

# 1) TheHive 서버의 URL과 API Key 설정
# 내부적으로 9000포트에 연결 
THEHIVE_URL = "http://localhost:9000/thehive"
API_KEY = "wyDwLtQqV7tkYCd5kETyuUPH5HQoDUor"

# 2) 요청에 필요한 헤더
headers = {
    "Content-Type": "application/json",
    "Authorization": f"Bearer {API_KEY}"
}

# 3) Case 생성 시 필요한 데이터(예: 제목, 심각도 등)
case_data = {
    "title": "Ransomware Incident Detected",
    "description": "Detected ransomware behavior on host X",
    "severity": 3,
    "tlp": 3,
    "flag": True,
    "tags": ["ransomware", "automated"]
}

# 4) Case 생성 요청
create_case_url = f"{THEHIVE_URL}/api/v1/case"
response = requests.post(create_case_url, headers=headers, data=json.dumps(case_data), verify=False)

if response.status_code == 201:
    created_case = response.json()
    case_id = created_case['_id']
    print(f"Case created successfully with ID: {case_id}")
    
    # 5) 생성된 Case에 Workbook(템플릿) 적용 (옵션)
    workbook_data = {
        "ids": [case_id],
        # "caseTemplate": "IRM-17-Ransomware",
        "caseTemplate": "~40964120",
        # "caseTemplate": "시스템 공격",
        "importTasks": ["~12336", "~12456", "~40964112", "~40968216", "~40972312", "~40972448"],
    }
    apply_workbook_url = f"{THEHIVE_URL}/api/v1/case/_bulk/caseTemplate"
    wb_response = requests.post(apply_workbook_url, headers=headers, data=json.dumps(workbook_data))

    if wb_response.status_code == 204:
        print("Workbook template applied successfully!")
    else:
        print(f"Failed to apply workbook template. Error: {wb_response.text}")
else:
    print(f"Failed to create case. Error: {response.text}")
