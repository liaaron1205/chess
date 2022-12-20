import json
import subprocess

def run_test(input_fen, expected_fens):
  # Run the test executable, providing the input through stdin
  process = subprocess.Popen(['./test'], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
  stdout, _ = process.communicate(input_fen)

  # Split the output into a list of lines
  actual_fens = stdout.split('\n')

  # Remove any pure whitespace outputs
  actual_fens = [x for x in actual_fens if x.strip()]

  # Compare the expected and actual outputs
  if set(expected_fens) == set(actual_fens):
    return True, None
  else:
    return False, (expected_fens, actual_fens)

# Read test.json as an input
with open('test.json') as f:
  data = json.load(f)

# Iterate over the test cases
for i, test_case in enumerate(data['testCases']):
  input_fen = test_case['start']['fen']
  expected_fens = [x['fen'] for x in test_case['expected']]

  # Run the test and write a summary of the result
  success, result = run_test(input_fen, expected_fens)
  if success:
    print('Test {} passed:'.format(i+1))
    print('  input = {}'.format(input_fen))
  else:
    expected, actual = result
    print('Test {} failed:'.format(i+1))
    print('  input = {}'.format(input_fen))
    print('  expected but not actual:')
    for fen in set(expected) - set(actual):
      print('    {}'.format(fen))
    print('  actual but not expected:')
    for fen in set(actual) - set(expected):
      print('    {}'.format(fen))
  print('-' * 80)  # Add a separator between test cases
