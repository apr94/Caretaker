from django.conf.urls import url

from caretaker import views

urlpatterns = [
    url(r'^$', views.index, name='index'),
   # ex: /polls/5/results/

    url(r'^alert/$', views.alert, name='alert'),
    url(r'^request_ack/$', views.request_ack, name='request_ack'),
    url(r'^ack/$', views.ack, name='ack'),
    url(r'^set_loc/$', views.set_loc, name='set_loc'),
    url(r'^has_alert/$', views.has_alert, name='has_alert'),
    url(r'get_loc/$', views.get_loc, name='get_loc'),
    url(r'get_battery/$', views.get_battery, name='get_battery'),
    url(r'set_battery/$', views.set_battery, name='set_battery'),
    url(r'get_log/$', views.get_log, name='get_log'),
    url(r'del_logs/$', views.del_logs, name='del_logs'),
    url(r'^has_ack/$', views.has_ack, name='has_ack'),
    
    # url(r'^(?P<val>[0-9]+.[0-9]+)/high/$', views.high, name='high'),
    # ex: /polls/5/vote/
    # url(r'^(?P<val>[0-9]+.[0-9]+)/low/$', views.low, name='low'),
]
