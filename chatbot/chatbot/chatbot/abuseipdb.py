import os
import requests
from dotenv import load_dotenv

load_dotenv()

api_key = os.getenv('ABUSEIPDB_API_KEY')

def check_abuseipdb(query_item, query_type):
    """ AbuseIPDB API """
    result = {}
    if query_type == 'ip':
        url = 'https://api.abuseipdb.com/api/v2/check'
        querystring = {
            'ipAddress': query_item,
            'maxAgeInDays': '90'
        }
        headers = {
            'Accept': 'application/json',
            'Key': api_key
        }
        try:
            response = requests.get(url, headers=headers, params=querystring)
            response.raise_for_status()  # Raises a HTTPError if the status is 4xx, 5xx
            data = response.json()
            result = {
                'abuseConfidenceScore': data['data']['abuseConfidenceScore'],
                'totalReports': data['data']['totalReports'],
                'countryCode': data['data']['countryCode'],
                'usageType': data['data']['usageType'],
                'isp': data['data']['isp'],
                'domain': data['data']['domain'],
                'lastReportedAt': data['data']['lastReportedAt']
            }
        except requests.exceptions.RequestException as e:
            result = {"error": str(e)}
    else:
        result = {"error": f"Query type '{query_type}' is not supported. Only 'ip' is currently supported."}
    
    return result

# Uncomment the following lines to test the function
# result = check_abuseipdb('1.1.1.1', 'ip')
# print(result)