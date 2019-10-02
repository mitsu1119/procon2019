import requests
import json


# マッチID
ID = 6

headers = {
        'Authorization': 'procon30_example_token',
        }

url = 'http://localhost:8081/matches/' + str(ID)
response = requests.get( url, headers=headers)

map_data = response.json()

# jsonデータをファイル書き込み
#fw = open('fields.json', 'w')
fw = open('../../public_field/fields.json', 'w')

json.dump(map_data, fw, ensure_ascii=False, indent=2)

fw.close()



