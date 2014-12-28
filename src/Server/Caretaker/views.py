from django.shortcuts import render
from django.http import HttpResponse
from django.conf import settings
from django.core.mail import send_mail
from time import gmtime, strftime
import shelve

# the API endpoint

def index(request):

    return HttpResponse("You have reached the emailer API endpoint\n")

# delete logs 

def del_logs(request):

    s = shelve.open('store.db')
	s['logs'] = ""
	return HttpResponse("LD")

# send alert, also send email

def alert(request):
    send_mail('Caretaker App', 
	    'Hi!\n\nAn Alert has been sent by the care receiver. You might want to check up on them!\n\nThanks!\n -CareTaker by Palimar', 
	    settings.EMAIL_HOST_USER, 
	    ['send_email@gmail.com'], 
	    fail_silently=True)

    s = shelve.open('store.db')
	try:
	    s['alert'] = s['loc']
		x = s.get('logs', "")
		x = x + "#" + (str(strftime("%Y-%m-%d %H:%M:%S", gmtime()) + ": ALERT at " + s['loc']))
		s['logs'] = x

	finally:
	    s.close()
	return HttpResponse(str(x))

# acknowledge the care receiver, digital handshaking

def ack(request):
    send_mail('Caretaker App', 
	    'Hi!\n\nYour Acknowledge Request has been fulfilled by the care receiver!\n\nThanks!\n -CareTaker by Palimar', 
	    settings.EMAIL_HOST_USER, 
	    ['send_email@gmail.com'], 
	    fail_silently=False)
    s = shelve.open('store.db')
	try:
	    s['ack'] = 1
		s['request_ack'] = 0;
	finally:
	    s.close()
	return HttpResponse("1")

# set the location to the params

def set_loc(request):

    val = request.GET.get('loc', ',')

	send_mail('Caretaker App', 
		'Hi!\n\nLocation of care receiver has been updated to ' + val + '. \n\nThanks!\n -CareTaker by Palimar', 
		settings.EMAIL_HOST_USER, 
		['send_email@gmail.com'], 
		fail_silently=True)
	s = shelve.open('store.db')
	try:
	    s['loc'] = val
	finally:
	    s.close()
	return HttpResponse(val)

# request an acknowledgement or handshake

def request_ack(request):

    send_mail('Caretaker App', 
	    'Hi!\n\nA request for acknowledgement has been sent to the care receiver. \n\nThanks!\n -CareTaker by Palimar', 
	    settings.EMAIL_HOST_USER, 
	    ['send_email@gmail.com'], 
	    fail_silently=True)
    s = shelve.open('store.db')
	try:
	    s['request_ack'] = 1
	finally:
	    s.close()
	return HttpResponse("1")

#Check if there is an alert, if so address it and reset address flag

def has_alert(request):
    s = shelve.open('store.db')
	a = s['alert']
	s['alert'] = "0"
	return HttpResponse(a)

# check if ack is present, if so address it and reset address flag

def has_ack(request):
    s = shelve.open('store.db')
	a = s['ack']
	s['ack'] = 0
	return HttpResponse(a)

# set battery level

def set_battery(request):
    val = request.GET.get('bat', ',')
	s = shelve.open('store.db')
	s['battery'] = val 

	val = int(val.split(",")[2])

	if(val < 5):
	    send_mail('Caretaker App', 
		    'Hi!\n\nWith current usage, the Pebble will stop functioning in 1 hour. You might want to alert the care receiver to recharge the device. \n\nThanks!\n -CareTaker by Palimar', 
		    settings.EMAIL_HOST_USER, 
		    ['send_email@gmail.com'], 
		    fail_silently=True)
	    return HttpResponse(val)

# get the last know location of the care receiver

def get_loc(request):
    s = shelve.open('store.db')
	return HttpResponse(s['loc'])

# get the logs so far

def get_log(request):
    s = shelve.open('store.db')
	return HttpResponse(s['logs'])

# get the battery info, also the cpu and memory

def get_battery(request):
    s = shelve.open('store.db')
	return HttpResponse(s['battery'])
