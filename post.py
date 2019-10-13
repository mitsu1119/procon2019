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
    'Authorization': '5cbd0878da3217118a9893bcab4aa864d9478aaccb25a94eac926cc3b401ae1a',
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
response = requests.post('http://10.10.52.252/matches/1/action', headers=headers, data=data)

print(response, file=sys.stderr)

print("[*] done", file=sys.stderr)


