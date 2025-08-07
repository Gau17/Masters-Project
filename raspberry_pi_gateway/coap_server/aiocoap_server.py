import datetime
import logging

import asyncio

import aiocoap.resource as resource
from aiocoap.numbers.contentformat import ContentFormat
import aiocoap


class Welcome(resource.Resource):
    representations = {
        ContentFormat.TEXT: b"Welcome to the demo server",
        ContentFormat.LINKFORMAT: b"</.well-known/core>,ct=40",
        # ad-hoc for application/xhtml+xml;charset=utf-8
        ContentFormat(65000): b'<html xmlns="http://www.w3.org/1999/xhtml">'
        b"<head><title>aiocoap demo</title></head>"
        b"<body><h1>Welcome to the aiocoap demo server!</h1>"
        b'<ul><li><a href="time">Current time</a></li>'
        b'<li><a href="whoami">Report my network address</a></li>'
        b"</ul></body></html>",
    }

    default_representation = ContentFormat.TEXT

    async def render_get(self, request):
        cf = (
            self.default_representation
            if request.opt.accept is None
            else request.opt.accept
        )
        try:
            return aiocoap.Message(payload=self.representations[cf], content_format=cf)
        except KeyError:
            raise aiocoap.error.UnsupportedContentFormat


class BlockResource(resource.Resource):
    """Example resource which supports the GET and PUT methods. It sends large
    responses, which trigger blockwise transfer."""

    def __init__(self):
        super().__init__()
        self.set_content(           
            b"This is the resource's default content. It is padded "
            b"with numbers to be large enough to trigger blockwise "
            b"transfer.\n"
        )

    def set_content(self, content):
        self.content = content
        while len(self.content) <= 1024:
            self.content = self.content + b"0123456789\n"

    async def render_get(self, request):
        return aiocoap.Message(payload=self.content, content_format=ContentFormat.TEXT)

    async def render_put(self, request):
        print("PUT payload: %s" % request.payload)
        self.set_content(request.payload)
        return aiocoap.Message(code=aiocoap.CHANGED, payload=self.content)


class SeparateLargeResource(resource.Resource):
    """Example resource which supports the GET method. It uses asyncio.sleep to
    simulate a long-running operation, and thus forces the protocol to send
    empty ACK first."""

    def get_link_description(self):
        # Publish additional data in .well-known/core
        return dict(**super().get_link_description(), title="A large resource")

    async def render_get(self, request):
        await asyncio.sleep(3)

        payload = (
            "Three rings for the elven kings under the sky, seven rings "
            "for dwarven lords in their halls of stone, nine rings for "
            "mortal men doomed to die, one ring for the dark lord on his "
            "dark throne.".encode("ascii")
        )
        return aiocoap.Message(payload=payload, content_format=ContentFormat.TEXT)


class TimeResource(resource.ObservableResource):
    """Example resource that can be observed. The `notify` method keeps
    scheduling itself, and calles `update_state` to trigger sending
    notifications."""

    def __init__(self):
        super().__init__()

        self.handle = None

    def notify(self):
        self.updated_state()
        self.reschedule()

    def reschedule(self):
        self.handle = asyncio.get_event_loop().call_later(5, self.notify)

    def update_observation_count(self, count):
        if count and self.handle is None:
            print("Starting the clock")
            self.reschedule()
        if count == 0 and self.handle:
            print("Stopping the clock")
            self.handle.cancel()
            self.handle = None

    async def render_get(self, request):
        payload = datetime.datetime.now().strftime("%Y-%m-%d %H:%M").encode("ascii")
        return aiocoap.Message(payload=payload, content_format=ContentFormat.TEXT)


class WhoAmI(resource.Resource):
    async def render_get(self, request):
        text = ["Used protocol: %s." % request.remote.scheme]

        text.append("Request came from %s." % request.remote.hostinfo)
        text.append("The server address used %s." % request.remote.hostinfo_local)

        claims = list(request.remote.authenticated_claims)
        if claims:
            text.append(
                "Authenticated claims of the client: %s."
                % ", ".join(repr(c) for c in claims)
            )
        else:
            text.append("No claims authenticated.")

        return aiocoap.Message(
            payload="\n".join(text).encode("utf8"),
            content_format=ContentFormat.TEXT,
        )

class BasicResource(resource.Resource):
    """
    A simple resource that supports GET and PUT.
    GET returns the current content.
    PUT updates the content.
    """
    def __init__(self):
        super().__init__()
        self.content = b" "

    async def render_get(self, request):
        """Handles GET requests by returning the current content."""
        return aiocoap.Message(payload=self.content, content_format=ContentFormat.TEXT)

    async def render_put(self, request):
        """Handles PUT requests by updating the resource's content."""
        payload_text = request.payload.decode('utf-8')
        print(f"PUT request received. New content: '{payload_text}'")
        
        self.content = request.payload
        
        # Respond with a 2.04 Changed code to indicate success.
        return aiocoap.Message(code=aiocoap.CHANGED, payload=self.content, content_format=ContentFormat.TEXT)
    
    async def render_post(self, request):
        """Handles POST requests by appending to the resource's content."""
        print(f"POST request received. Appending: '{request.payload.decode()}'")
        self.content += b" " + request.payload
        # Respond with 2.04 Changed to indicate success
        return aiocoap.Message(code=aiocoap.CHANGED, payload=self.content, content_format=ContentFormat.TEXT)
    
import json

class SensorDataResource(resource.Resource):
    """
    A resource to accept binary sensor data via POST and display it via GET.
    """
    def __init__(self):
        super().__init__()
        self.stored_data = []

    async def render_post(self, request):
        """Accepts a binary payload and stores it."""
        payload = request.payload
        
        print(f"Received sensor data: {payload.hex()}")
        
        self.stored_data.append(payload)
        
        # Respond with 2.01 Created, indicating success
        response_payload = f"Data record #{len(self.stored_data)} stored successfully.".encode('utf-8')
        return aiocoap.Message(code=aiocoap.CREATED, payload=response_payload)

    async def render_get(self, request):
        """Returns all stored sensor data records as a JSON array."""
        hex_data = [d.hex() for d in self.stored_data]
        
        response_payload = json.dumps({"sensor_records": hex_data}).encode('utf-8')
        
        return aiocoap.Message(payload=response_payload, content_format=ContentFormat.JSON)

# logging setup

logging.basicConfig(level=logging.INFO)
logging.getLogger("coap-server").setLevel(logging.DEBUG)


async def main():
    # Resource tree creation
    root = resource.Site()

    root.add_resource(
        [".well-known", "core"], resource.WKCResource(root.get_resources_as_linkheader)
    )
    root.add_resource([], Welcome())
    root.add_resource(["time"], TimeResource())
    root.add_resource(["other", "block"], BlockResource())
    root.add_resource(["other", "separate"], SeparateLargeResource())
    root.add_resource(["whoami"], WhoAmI())
    root.add_resource(["basic"], BasicResource())
    root.add_resource(["sensor", "data"], SensorDataResource())

    await aiocoap.Context.create_server_context(root)

    # Run forever
    await asyncio.get_running_loop().create_future()


if __name__ == "__main__":
    asyncio.run(main())
