import json

with open('Config/UsersInfo.json') as f:
    data = json.load(f)
    print(data)
