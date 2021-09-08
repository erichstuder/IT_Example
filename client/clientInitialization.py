def mySend(text):
	print('send: "' +  text + '"\n')
	send(text)   


send('set connectionType USB_RS232')
send('set VID 2341')
send('set PID 8036')

mySend('reset')
mySend('log desiredValue')
mySend('log actualValue')
