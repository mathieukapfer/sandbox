import re

test_strings = ['555-1212', 'ILL-EGAL']

for test_string in test_strings:
    if re.match('^\d{3}-\d{4}$', test_string):
        print test_string, 'match'
    else:
        print test_string, 'do not match'
        

