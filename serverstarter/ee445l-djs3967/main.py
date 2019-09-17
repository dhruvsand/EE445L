#### Script for EdX course UT6.02X
## Author: Ramesh Yerraballi
## Date: Spring 2015
## License: Freeware
## The code here is based on three sources:
##  1. The basic guestbook appengine application from Google
##  2. pymaps.py script from: https://code.google.com/p/pymaps/
##  3. ipinfo.io's ip locator is more accurate than geobytes http://ipinfo.io/
##  4. geobytes.com's ip locator at http://www.geobytes.com/iplocator/
##     and their json api
## Need the next line to deal with strings that the geobytes server returns that
## have unicode characters -- Did not Work though
#-*- coding: UTF-8 -*-
import cgi
import time
import logging
import urllib2
import unicodedata
import webapp2
import random
import json
from google.appengine.ext import ndb

DEFAULT_LOGBOOK_NAME = 'logbook'

# We set a parent key on the 'Greetings' to ensure that they are all
# in the same entity group. Queries across the single entity group
# will be consistent.  However, the write rate should be limited to
# ~1/second.

def logbook_key(logbook_name=DEFAULT_LOGBOOK_NAME):
    """Constructs a Datastore key for a logbook entity.

    We use logbook_name as the key.
    """
    return ndb.Key('logbook', logbook_name)


# [START greeting]
class Greeting(ndb.Model):
    """A main model for representing an individual Guestbook entry."""
    author = ndb.StringProperty(indexed=False)
    greet = ndb.StringProperty(indexed=False)
    date = ndb.DateTimeProperty(auto_now_add=True)
    city = ndb.StringProperty(indexed=False)
    ipaddr = ndb.StringProperty(indexed=False)
# [END greeting]
MAIN_PAGE_HTML = """\
<html>
  <body>
    <form action="/sign" method="post">
	  <div><input type="text" name="uid" cols="4">
	  <div><input type="text" name="greet" cols="30">
    </form>
  </body>
</html>
"""


class MainPage(webapp2.RequestHandler):
    def get(self):
        self.response.write('</body></html>')
        self.response.write('<h2>Embedded Systems - University of Texas at Austin</h2>')
        self.response.write('<hr>')
        # [START query]
        greetings_query = Greeting.query(
            ancestor=logbook_key('logbook')).order(-Greeting.date)
        greetings = greetings_query.fetch()
        # [END query]
        
        for greeting in greetings:
            self.response.write('<b>%s@%s</b>: <i>%s</i> (accessed from %s)' %
                                        (greeting.author, greeting.city, greeting.greet, greeting.ipaddr))
            self.response.write('<hr>')

class Auto(webapp2.RequestHandler):
    def get(self):
        #comment these three lines for debugging; So we can access it through a browser
        #if self.request.environ.get('HTTP_USER_AGENT') != 'Keil':
        #   self.response.write('<html><body> <pre> Invalid Access</pre> </body></html>')
        #   return
        greeting = Greeting(parent=logbook_key('logbook'))
        greeting.ipaddr = self.request.environ.get('REMOTE_ADDR')     
        geo_url = "http://ipinfo.io/%s/loc" % greeting.ipaddr
        # There should be some error checking here if the lookup fails
        json_stream = urllib2.urlopen(geo_url)
        retstr = json_stream.read()
        greeting.author = cgi.escape(self.request.get('id'))
        greeting.city = cgi.escape(self.request.get('city'))
        greeting.greet = cgi.escape(self.request.get('greet'))
        greeting.put()
        self.response.write('<html><body>')
        self.response.write('<pre>{"%s":"%s", "%s":"%s"}</pre>' %
                                  ("id", greeting.author, "greet", greeting.greet))
        self.response.write('</body></html>')

application = webapp2.WSGIApplication([
  ('/', MainPage),
	('/query', Auto)
], debug=True)
