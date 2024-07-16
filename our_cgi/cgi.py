import sys


def main():
    if len(sys.argv) == 2:
        with open(sys.argv[1], 'r') as file:
            data = file.read().replace('a', 'b')

        print("Content-type: text/html; charset=UTF-8\r\n\r\n" + data)
        while True:
            print("a")
if __name__=="__main__":
    main()
