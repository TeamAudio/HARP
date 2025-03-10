# -*- coding: utf-8 -*-
import argparse
from gradio_client import Client
from pathlib import Path
import json
import shutil
import signal
import time

client = None
def handler_stop_signals(signum, frame):
    global client
    print("Stopping client...")
    if client is not None:
        # send a cancel request to the server
        client.submit(api_name="/wav2wav-cancel")
        print(f"Sent cancel request to {client.url}. Exiting...")
    else:
        print("Client was None. Exiting...")

signal.signal(signal.SIGINT, handler_stop_signals)
signal.signal(signal.SIGTERM, handler_stop_signals)

def main(
        url: str, 
        output_path: str, 
        mode: str, 
        ctrls_path : str = None, 
        ctrls_timeout: float = 30,
        cancel_flag_path: str = None,
        status_flag_path: str = None
    ):
    assert url, "Please specify a url to connect to."
    assert output_path, "Please specify an output path."
    global client
    client = Client(url)
    
    if mode == "get_ctrls":
        print(f"Getting controls for {url}...")
        # ctrls will be a dict, instead of a path now
        # ctrls = client.predict(api_name="/wav2wav-ctrls")
        job = client.submit(api_name="/wav2wav-ctrls")

        # wait for the job to finish or timeout
        start_time = time.time()
        while not job.done():
            time.sleep(0.05)
            if time.time() - start_time > ctrls_timeout:
                raise TimeoutError(f"Timeout reached while waiting for controls from {url}.")

        # done
        ctrls = job.result()
        print(f"got ctrls: {ctrls}")
        # if it's a string, it's a filepath
        if isinstance(ctrls, str):
            shutil.move(ctrls, output_path)
        # if it's not, likely that it's the actual controls
        else:
            print(f"Saving ctrls to {output_path}...")
            with open(output_path, "w") as f:
                json.dump(ctrls, f)

    elif mode == "predict":
        assert ctrls_path is not None, "Please specify a ctrls path."
        with open(ctrls_path) as f:
            ctrls = json.load(f)
            assert isinstance(ctrls, list), "Controls must be a list of parameter values."
            print(f"loaded ctrls: {ctrls}")
        print(f"Predicting audio for {url}...")
        job = client.submit(*ctrls, api_name="/wav2wav")

        canceled = False
        while not job.done():
            # check if the cancel flag exists
            # if it does, cancel the job
            if cancel_flag_path is not None:
                if Path(cancel_flag_path).exists():
                    print("Cancel flag detected. Cancelling...")
                    client.submit(api_name="/wav2wav-cancel")
                    canceled = True
                    Path(status_flag_path).write_text("Status.CANCELED")
                    break
            
            # check if we were given a status path
            # if it does, write the status to the file
            status = job.status()
            print(f"Status: {status}")

            if status_flag_path is not None:
                Path(status_flag_path).write_text(str(status.code))
        
            time.sleep(0.05)
        
        if not canceled:
            audio_path = job.result()

            print(f"Saving audio to {output_path}...")
            shutil.move(audio_path, output_path)
        else:
            # still consume the result and block? 
            # job.result()
            pass

    else:
        raise ValueError("Invalid mode. Choose either 'get_ctrls' or 'predict'.")

    print("gradiojuce_client done! :)")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Process some arguments.')
    parser.add_argument('--url', required=True, help='The URL to connect to.')
    parser.add_argument('--output_path', required=True, help='The output path to save the file.')
    parser.add_argument('--mode', required=True, choices=['get_ctrls', 'predict'], help='The mode of operation.')
    parser.add_argument('--ctrls_path', help='The path to the controls file.')
    parser.add_argument('--cancel_flag_path', help='The path to the cancel flag file.')
    parser.add_argument('--status_flag_path', help='The path to the status flag file.')
    parser.add_argument('--ctrls_timeout', type=float, default=30, help='The timeout for getting controls.')
    
    args = parser.parse_args()
    
    try:
        # Redirect standard output to a file with UTF-8 encoding
        sys.stdout = open(args.output_path.replace("control_spec.json","control_spec_temp.json"), 'w', encoding='utf-8')
        # Print or log the problematic string for debugging
        print(f"Parse Args: {args}")
        # Call the main function
        main(**vars(args))
    except Exception as e:
        # Print the exception for debugging
        print(f"Exception: {e}")
        raise
