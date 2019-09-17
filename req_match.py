import requests
import json

# リクエストを送りマッチデータを取得
headers = {
        'Authorization': 'procon30_example_token',
        }

response = requests.get('http://localhost:8081/matches', headers=headers)
    
match_data = response.json()

# 自分の高専を設定
my_kosen = 'C高専'

# マッチ数を取得
number_of_team = len(match_data)

# 自分の高専がどの要素にあるのかを計算
for i in range(number_of_team):
    if match_data[i]['matchTo'] == mykosen:
        my_match = i

# 自分のマッチのみを取得
match = match_data[my_match]

# ----- デバッグ用 ------------
# 自分のマッチIDを取得
match_id = match['id']
# ターンの秒数を取得
turn_time = match['turnMillis']
# ターンを秒に
turn_time_sec = int(turn_time/1000)
# 終了ターン取得
end_turn = match['turns']
# -----------------------------

# jsonデータをファイル書き込み
fw = open('matches.json', 'w')

json.dump(match, fw, ensure_ascii=False, indent=2)

fw.close()

