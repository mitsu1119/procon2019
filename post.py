import sys
import json
import requests

print("[*] POST: action data...", file=sys.stderr)

f = open('../../action.json','r')
post_data = json.load(f)

f.close()

# サーバへの送信処理 -------------------------------------

# 本戦用
headers = {
    'Authorization': 'procon30_example_token',
    'Content-Type': 'application/json',
}

# テスト用
#headers = {
#	'Content-Type': 'application/#json',
#}

# サンプル
#data = '{"actions":[{"agentID":2,"dx":1,"dy":1,"type":"move"}]}'

# 本戦データ
data = json.dumps(post_data).encode("utf-8")

# 簡易サーバ用
#response = requests.post('http://localhost:8081/matches/1/action', headers=headers, data=data)

# 本戦用URLを入れる
response = requests.post('http://172.16.18.243:80/index.php', headers=headers, data=data)

#print(response.text, file=sys.stderr)

print("[*] done", file=sys.stderr)


