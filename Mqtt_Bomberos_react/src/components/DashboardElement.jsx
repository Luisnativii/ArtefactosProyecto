function DashboardElement({ title, value, unit }) {
  return (
    <div className="p-6 rounded-md bg-slate-100 text-neutral-800 flex flex-col gap-2 justify-center items-center w-48">
      <h2 className="font-bold text-lg">{title}</h2>
      <p className="text-2xl">
        {value ? value : "Sin datos"} {unit && <span className="text-sm">{unit}</span>}
      </p>
    </div>
  );
}

export default DashboardElement;
