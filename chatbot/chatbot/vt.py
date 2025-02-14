import os
from dotenv import load_dotenv
import virustotal3.core

load_dotenv()

api_key = os.getenv('VIRUS_TOTAL_API_KEY')

def virustotal(query_item, query_type):
    """ virustotal api """
    result = {}
    try:
        if query_type == 'ip':
            virus_total = virustotal3.core.IP(api_key)
            response = virus_total.info_ip(query_item)
            
            if 'data' in response and 'attributes' in response['data']:
                attrs = response['data']['attributes']
                result = {
                    'last_analysis_stats': attrs.get('last_analysis_stats', {}),
                    'country': attrs.get('country', ''),
                    'as_owner': attrs.get('as_owner', ''),
                    'network': attrs.get('network', ''),
                    'last_analysis_date': attrs.get('last_analysis_date', ''),
                    'reputation': attrs.get('reputation', 0),
                    'total_votes': attrs.get('total_votes', {}),
                }
                
                # Add Whois information if available
                if 'whois' in attrs:
                    whois_info = attrs['whois'].split('\n')
                    for line in whois_info:
                        if line.startswith('OrgName:'):
                            result['isp'] = line.split(':', 1)[1].strip()
                        elif line.startswith('Organization:'):
                            result['organization'] = line.split(':', 1)[1].strip()
        elif query_type == 'domain':
            virus_total = virustotal3.core.Domains(api_key)
            result = virus_total.info_domain(query_item)
        elif query_type == 'url':
            virus_total = virustotal3.core.URL(api_key)
            result = virus_total.info_url(query_item)
        elif query_type == 'hash':
            virus_total = virustotal3.core.Files(api_key)
            result = virus_total.info_file(query_item)
        else:
            result = {"error": f"Query type '{query_type}' is not supported."}
    except Exception as e:
        result = {"error": str(e)}
    
    return result

# Uncomment the following lines to test the function
# result = virustotal('1.1.1.1', 'ip')
# print(result)

# import os
# from splunklib.searchcommands import dispatch, GeneratingCommand, Option, Configuration
# import virustotal3.core
# import json

# from dotenv import load_dotenv
# # from config import virus_total_api_key as api_key

# load_dotenv()

# api_key = os.getenv('VIRUS_TOTAL_API_KEY')

# def virustotal(query_item, query_type):
#     """ virustotal api """
#     result = {}
#     if query_type == 'ip':
#         virus_total = virustotal3.core.IP(api_key)
#         result = virus_total.info_ip(query_item)
#     elif query_type == 'domain':
#         virus_total = virustotal3.core.Domians(api_key)
#         result = virus_total.info_domain(query_item)
#     elif query_type == 'url':
#         virus_total = virustotal3.core.URL(api_key)
#         result = virus_total.info_url(query_item)
#     elif query_type == 'hash':
#         virus_total = virustotal3.core.Files(api_key)
#         result = virus_total.info_file(query_item)
#     if 'data' in result and 'attributes' in result['data']:
#         return result['data']['attributes']['last_analysis_stats']
#     else:
#         return result
    
# result = virustotal('1.1.1.1', 'ip')
# print(result)
