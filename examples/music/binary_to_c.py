
bytes_read = open("test.mid", "rb").read()
print len(bytes_read)
for b in bytes_read:
    print str(ord(b)) + ",",
