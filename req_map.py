import requests
import json


# マッチID
ID = 6

headers = {
        'Authorization': '5cbd0878da3217118a9893bcab4aa864d9478aaccb25a94eac926cc3b401ae1a',
        }

url = 'http://10.10.52.252/matches/' + str(ID)
response = requests.get( url, headers=headers)

map_data = response.json()

# jsonデータをファイル書き込み
#fw = open('fields.json', 'w')
fw = open('../../public_field/fields.json', 'w')

json.dump(map_data, fw, ensure_ascii=False, indent=2)

fw.close()



