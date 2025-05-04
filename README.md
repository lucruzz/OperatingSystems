# Operating Systems Project

## To run
To run this program open a terminal to be the server and type:
```
./bin/server 20 5000
```
Open a second terminal representing the client and enter the command:
```
./bin/client clients/client1/ 5000
```

## Sites for testing
```
search http://web.ist.utl.pt/luis.tarrataca/hello.html
```

```
search http://web.ist.utl.pt/luis.tarrataca/file1.html
```

```
search http://web.ist.utl.pt/luis.tarrataca/file2.html
```

```
search http://web.ist.utl.pt/luis.tarrataca/file3.html
```
```
search http://web.ist.utl.pt/luis.tarrataca/file4.html
```
```
search http://web.ist.utl.pt/luis.tarrataca/file5.html
```

```
search http://web.ist.utl.pt/luis.tarrataca/hello.html http://web.ist.utl.pt/luis.tarrataca/file1.html
```

```
search http://web.ist.utl.pt/luis.tarrataca/file1.html http://web.ist.utl.pt/luis.tarrataca/file2.html
```

```
search http://web.ist.utl.pt/luis.tarrataca/file2.html http://web.ist.utl.pt/luis.tarrataca/file3.html
```

```
search http://web.ist.utl.pt/luis.tarrataca/hello.html http://web.ist.utl.pt/luis.tarrataca/file1.html http://web.ist.utl.pt/luis.tarrataca/file2.html http://web.ist.utl.pt/luis.tarrataca/file3.html http://web.ist.utl.pt/luis.tarrataca/file4.html http://web.ist.utl.pt/luis.tarrataca/file5.html
```

## Checking if everything is okay

```
diff originals/hello_original.html proxy/hello.html
diff originals/file1_original.html proxy/file1.html
diff originals/file2_original.html proxy/file2.html
diff originals/file3_original.html proxy/file3.html
diff originals/file4_original.html proxy/file4.html
diff originals/file5_original.html proxy/file5.html


diff originals/hello_original.html clients/client1/hello.html
diff originals/file1_original.html clients/client1/file1.html
diff originals/file2_original.html clients/client1/file2.html
diff originals/file3_original.html clients/client1/file3.html
diff originals/file4_original.html clients/client1/file4.html
diff originals/file5_original.html clients/client1/file5.html

diff originals/hello_original.html clients/client2/hello.html
diff originals/file1_original.html clients/client2/file1.html
diff originals/file2_original.html clients/client2/file2.html
diff originals/file3_original.html clients/client2/file3.html
diff originals/file4_original.html clients/client2/file4.html
diff originals/file5_original.html clients/client2/file5.html

diff originals/hello_original.html clients/client3/hello.html
diff originals/file1_original.html clients/client3/file1.html
diff originals/file2_original.html clients/client3/file2.html
diff originals/file3_original.html clients/client3/file3.html
diff originals/file4_original.html clients/client3/file4.html
diff originals/file5_original.html clients/client3/file5.html
```
