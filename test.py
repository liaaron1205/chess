import json
import subprocess

process = subprocess.Popen(['test'], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
stdout, _ = process.communicate(input=input_fen)