import requests


# マッチID
n = 3

headers = {
        'Authorization': 'procon30_example_token',
        }

req = 'http://localhost:8081/matches/' + str(n)
response = requests.get( req, headers=headers)

map_data = response.json()

# jsonデータをファイル書き込み
fw = open('fields.json', 'w')

json.dump(map_data, fw, ensure_ascii=False, indent=2)

fw.close()


