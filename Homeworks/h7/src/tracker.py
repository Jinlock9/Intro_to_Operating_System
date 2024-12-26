from bcc import BPF

with open("ex2_2.c", "r") as f: # or ex7_3.c
    bpf_text = f.read()

b = BPF(text=bpf_text)

if "on_sys_clone" in bpf_text:
    b.attach_kprobe(event="do_fork", fn_name="on_sys_clone")
    b.attach_kretprobe(event="do_fork", fn_name="on_sys_clone_ret")
elif "sched_process_fork" in bpf_text:
    b.attach_tracepoint(tp="sched:sched_process_fork", fn_name="sched_process_fork")

print("Tracking subprocesses... Hit Ctrl-C to end.")
b.trace_print()
