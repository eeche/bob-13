import os
from dotenv import load_dotenv

load_dotenv()

config = {
    'dbpassword': os.getenv('DB_PASSWORD'),
    'log': os.getenv('LOG_LVL', 'INFO')
}

# import os.path
# import json
# import os

# if os.path.isfile('/app/conf/conf.json') is False:
#     with open('./conf/conf.json', 'w') as newconf:
#         conf = json.load(newconf)
#         conf['1234']  = os.environ['dbpassword']
#         conf['log']  = os.environ['LOG_LVL']
#         json.dump(conf, newconf, indent=4)

# with open('./conf/conf.json', 'r') as mainconf:
#     conf = json.load(mainconf)
