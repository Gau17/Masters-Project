import asyncio, cbor2, aiocoap, socket, random, time
from aiocoap import resource, Message, CONTENT, CHANGED

def get_primary_ip():
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(("8.8.8.8", 80))      # dummy external target
        ip = s.getsockname()[0]
        s.close()
        return ip
    except:
        return "0.0.0.0"

def list_local_ips():
    ips = set()
    for fam, _, _, _, sockaddr in socket.getaddrinfo(socket.gethostname(), None):
        if fam == socket.AF_INET:
            ips.add(sockaddr[0])
    return ips

class ArmPoseResource(resource.ObservableResource):
    def __init__(self):
        super().__init__()
        self.pose = {"waist":0,"shoulder":0,"elbow":0,"wrist_pitch":0,"wrist_roll":0,"gripper":0}
        self._observer_count = 0

    async def add_observation(self, request, server_observation):
        self._observer_count += 1
        peer = request.remote.hostinfo if request.remote else "unknown"
        print(f"[OBSERVE] New observer from {peer} (total={self._observer_count})")
        
        def on_cancel(obs):
            self._observer_count -= 1
            print(f"[OBSERVE] Observer from {peer} removed (remaining={self._observer_count})")

        server_observation.accept(on_cancel)

    async def render_get(self, request):
        print(f"[GET] /arm/pose (observe={'yes' if request.opt.observe is not None else 'no'}) "
              f"peer={request.remote.hostinfo if request.remote else 'unknown'}")
        payload_data = self.pose.copy()
        payload_data["server_timestamp"] = int(time.time() * 1000000)
        return Message(code=CONTENT, payload=cbor2.dumps(payload_data))

    async def render_post(self, request):
        try:
            new_pose = cbor2.loads(request.payload)
            for k in self.pose:
                if k in new_pose:
                    self.pose[k] = int(new_pose[k])
            print(f"[POST] Pose updated -> {self.pose} (notify {self._observer_count})")
            self.updated_state()
            return Message(code=CHANGED)
        except Exception as e:
            return Message(code=aiocoap.BAD_REQUEST, payload=str(e).encode())

# â¨ NEW: This function runs as a background task
async def update_pose_periodically(resource: ArmPoseResource):
    """A background task that updates the arm pose every 15 seconds."""
    while True:
        await asyncio.sleep(3)
        
        # Update pose with new random values (0-120 degrees)
        for joint in resource.pose:
            resource.pose[joint] = random.randint(0, 40)
        
        print(f"[AUTO-UPDATE] Pose updated -> {resource.pose}")
        
        # Notify all observers of the change
        resource.updated_state()

class PingResource(resource.Resource):
    """
    A simple resource that responds to a GET request instantly with an empty
    payload. This is ideal for allowing a client to measure RTT.
    """
    async def render_get(self, request):
        print(f"[PING] Received ping from {request.remote.hostinfo if request.remote else 'unknown'}")
        # Immediately return an empty success message
        return Message(code=CONTENT, payload=b'')

async def main():
    # Create the resource instance first
    arm_resource = ArmPoseResource()
    ping_resource = PingResource()

    root = resource.Site()
    root.add_resource(['arm','pose'], arm_resource)
    root.add_resource(['ping'], ping_resource)

    primary_ip = get_primary_ip()
    all_ips = list_local_ips()
    print(f"[SERVER] Starting CoAP on 0.0.0.0:5683")
    print(f"[SERVER] Primary IP: {primary_ip}")
    print(f"[SERVER] Hostname-resolved IPv4 addresses: {', '.join(all_ips) if all_ips else 'none'}")

    await aiocoap.Context.create_server_context(root, bind=('0.0.0.0', 5683))
    
    # â¨ NEW: Start the background task
    print("[SERVER] Starting periodic pose update task.")
    asyncio.create_task(update_pose_periodically(arm_resource))

    print("[SERVER] Ready (resource: /arm/pose)")
    await asyncio.get_event_loop().create_future() # Run forever

if __name__ == "__main__":
    asyncio.run(main())