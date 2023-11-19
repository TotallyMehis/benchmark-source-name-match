import sys
import re

def main():
    if len(sys.argv) < 2:
        print('Give path to a map file.')
        return
    mappath = sys.argv[1]
    content = None
    with open(mappath, 'rb') as fp:
        content = fp.read()
    result = re.findall(b'"targetname" "(.+)"', content)
    filename = 'names.txt'
    with open(filename, 'wb') as fp:
        for r in result:
            fp.write(r)
            fp.write(b'\n')
    print('Wrote', len(result), 'entity names to file', filename)

if __name__ == '__main__':
    main()
