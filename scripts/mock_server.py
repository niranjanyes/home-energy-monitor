from flask import Flask, request, jsonify
from flask_cors import CORS
import time

app = Flask(__name__)
CORS(app)

@app.route('/api/readings', methods=['POST'])
def receive_readings():
    data = request.json
    print(f"Received readings: {data}")
    
    # Simulate processing delay
    time.sleep(0.1)
    
    # Return success response
    return jsonify({
        "status": "success",
        "message": "Readings received",
        "timestamp": time.time(),
        "count": len(data['readings'])
    })

if __name__ == '__main__':
    app.run(port=3000, debug=True)
