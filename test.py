import sys
import json

f = open("../../action.json", "r")

data = json.load(f)

f.close()

print(data, file=sys.stderr)
